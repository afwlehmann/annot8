//
// PlaybackThread.cpp
// $Id$
//
// Copyright 2008 by Alexander Lehmann <lehmanna@in.tum.de>
//
// This file is part of annot8.
// 
// annot8 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// annot8 is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// annot8.  If not, see <http://www.gnu.org/licenses/>.
//


#include "PlaybackThread.h"
#include "SamplesPreviewCanvas.h"
#include "audio.h"
#include <cassert>


using std::min;


namespace annot8 {

namespace audio {


PlaybackThread::PlaybackThread(audio::Samples *samples, QObject *parent) :
    QThread(parent),
    _samples(samples),
    _audioSpec(0),
    _pos(0),
    _state(PlaybackThread::Pause)
{
    // Open the audio device.
    _audioSpec = new SDL_AudioSpec;
    _audioSpec->freq     = PlaybackThread::DefaultFrequency;
    _audioSpec->format   = AUDIO_S16SYS;
    _audioSpec->channels = 1;
    _audioSpec->samples  = 2048; // Values > 4096 cause time lags on Windows!
    _audioSpec->callback = pbCallback;
    _audioSpec->userdata = this;
    if (SDL_OpenAudio(_audioSpec, NULL) != 0) {
        delete _audioSpec;
        _audioSpec = 0;
        SDL_UnlockAudio();
        _mutex.unlock();
        throw audio::AudioException(SDL_GetError());
    }
}


PlaybackThread::~PlaybackThread()
{
    _mutex.lock();

    // Lock out the callback function, stop playback and unlock the
    // callback function again.
    SDL_LockAudio();
    SDL_PauseAudio(1);
    SDL_UnlockAudio();
    _state = PlaybackThread::Pause;

    // Now close the audio device and get rid of the allocated AudioSpec.
    SDL_CloseAudio();
    delete _audioSpec;
    _audioSpec = 0;

    _mutex.unlock();
}


void PlaybackThread::run()
{
    while (true) {
        _mutex.lock();
        if (_state == Pause || playbackPos() >= 1) {
            _mutex.unlock();
            break;
        }
        emit playbackPosChanged(playbackPos());
        _mutex.unlock();
        // 125ms gives a refresh rate of (theoretically) 8Hz.
        msleep(125);
    }
    // Qt emits the QThread::finished() signal right after run() has terminated.
}


void PlaybackThread::setPlaybackState(PlaybackState state)
{
    _mutex.lock();

    if (_state == state) {
        _mutex.unlock();
        return;
    }

    // Lock out the callback function.
    SDL_LockAudio();
    {
        if (state == Play) {
            SDL_PauseAudio(0);
            // Start playing.
            if (!isRunning())
                start();
        } else
            SDL_PauseAudio(1);
        // Eventually update the _state.
        _state = state;
    }
    // Unlock the callback function.
    SDL_UnlockAudio();

    _mutex.unlock();
}


float PlaybackThread::playbackPos() const
{
    return (float)_pos / (float)(_samples->numSamples - 1);
}


void PlaybackThread::setPlaybackPos(float pos)
{
    assert(pos >= 0 && pos <= 1);

    _mutex.lock();
    _pos = (size_t)(pos * (_samples->numSamples - 1));
    _mutex.unlock();
    emit playbackPosChanged(pos);
}


void PlaybackThread::pbCallback(void *user, Uint8 *buf, int size)
{
    PlaybackThread *pbt = static_cast<PlaybackThread *>(user);
    pbt->_mutex.lock();

    // We must pay attention to the fact that our samples are stored as 16-bit
    // signed shorts whereas the callback function's buffer deals with bytes.
    size_t bytesToCopy =
        min<size_t>((pbt->_samples->numSamples - pbt->_pos) * sizeof(short),
                    size);
    if (bytesToCopy) {
        memcpy(buf, (short *)pbt->_samples->ss->buffer + pbt->_pos, bytesToCopy);
        pbt->_pos += bytesToCopy / sizeof(short);
    }

    // Fill the possibly remaining space with silence.
    if (bytesToCopy < (size_t)size)
        memset(buf + bytesToCopy, pbt->_audioSpec->silence, size - bytesToCopy);

    pbt->_mutex.unlock();
}


} // namespace audio

} // namespace annot8
