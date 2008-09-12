//
// $Id$
//


#include "PlaybackThread.h"
#include "SamplesPreviewCanvas.h"
#include "audio.h"


using std::min;


namespace hiwi {

namespace audio {


PlaybackThread::PlaybackThread(audio::Samples *samples, QObject *parent) :
    QThread(parent),
    _samples(samples),
    _audioSpec(0),
    _pos(0),
    _state(PlaybackThread::Pause)
{
}


PlaybackThread::~PlaybackThread()
{
    _mutex.lock();

    // Before closing SDL's audio device and deleting the associated
    // SDL_AudioSpec, we must lock the audio device to assure that our
    // callback function doesn't interfere with this.
    SDL_LockAudio();
    if (_audioSpec) {
        SDL_PauseAudio(1);
        SDL_CloseAudio();
        delete _audioSpec;
        _audioSpec = 0;
    }
    SDL_UnlockAudio();

    _state = PlaybackThread::Pause;

    _mutex.unlock();
}


void PlaybackThread::run()
{
    while (true) {
        _mutex.lock();
        if (_state == Pause || playbackPos() >= 1) {
            _mutex.unlock();
            SDL_UnlockAudio();
            break;
        }
        emit playbackPosChanged(playbackPos());
        _mutex.unlock();
        // 100ms gives a refresh rate of 10Hz.
        msleep(100);
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
    if (state == Play) {
        // Open the audio device.
        assert(!_audioSpec);
        _audioSpec = new SDL_AudioSpec;
        _audioSpec->freq     = PlaybackThread::DefaultFrequency;
        _audioSpec->format   = AUDIO_S16SYS;
        _audioSpec->channels = 1;
        _audioSpec->samples  = 8192;
        _audioSpec->callback = pbCallback;
        _audioSpec->userdata = this;
        if (SDL_OpenAudio(_audioSpec, NULL) != 0) {
            delete _audioSpec;
            _audioSpec = 0;
            SDL_UnlockAudio();
            _mutex.unlock();
            throw audio::AudioException(SDL_GetError());
        }
        // Start playing.
        SDL_PauseAudio(0);
        if (!isRunning())
            start();
    } else if (state == Pause) {
        SDL_PauseAudio(1);
        SDL_CloseAudio();
        delete _audioSpec;
        _audioSpec = 0;
    }
    // Eventually update the _state and unlock the callback function.
    _state = state;
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

} // namespace hiwi
