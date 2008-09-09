//
// $Id$
//


#include "Sound.h"
#include "audio.h"
#include <SDL/SDL_audio.h>
#include <cassert>


namespace hiwi {

namespace audio {


Sound::Sound(const double *samples, size_t len, unsigned int sampleFreq) :
    _len(len),
    _sampleFreq(sampleFreq),
    _pos(0),
    _state(Pause),
    _audioSpec(NULL)
{
    // SDL limits the sample frequency from 11kHz to 44kHz.
    // TODO: Convert the samples?!
    assert(sampleFreq >= MinSampleFreq && sampleFreq <= MaxSampleFreq);

    // Allocate memory and copy the samples. The samples have to be converted
    // from the interval [-1, 1] to [-32768, 32767].
    _samples = new short[len];
    for (size_t i = 0; i < len; i++)
        _samples[i] = (short)((samples[i] + 1) / 2.0 * 65535 - 32768);
}


Sound::~Sound()
{
    SDL_LockAudio();
    if (_audioSpec) {
        SDL_PauseAudio(1);
        SDL_CloseAudio();
        delete _audioSpec;
        _audioSpec = NULL;
    }
    SDL_UnlockAudio();
    delete[] _samples;
    _samples = NULL;
}


void Sound::setPlaybackState(PlaybackState state)
{
    ASSERT_AUDIO_INITIALIZED

    if (state == _state)
        return;

    // Lock the callback function.
    SDL_LockAudio();
    do {
        if (state == Play) {
            // Open the audio device.
            assert(!_audioSpec);
            _audioSpec = new SDL_AudioSpec;
            _audioSpec->freq     = _sampleFreq;
            _audioSpec->format   = AUDIO_S16SYS;
            _audioSpec->channels = 1;
            _audioSpec->samples  = 4096;
            _audioSpec->callback = pbCallback;
            _audioSpec->userdata = this;
            if (SDL_OpenAudio(_audioSpec, NULL) != 0) {
                delete _audioSpec;
                _audioSpec = NULL;
                SDL_UnlockAudio();
                throw AudioException(SDL_GetError());
            }
            // Start playing.
            SDL_PauseAudio(0);
        } else if (state == Pause) {
            SDL_PauseAudio(1);
            SDL_CloseAudio();
            delete _audioSpec;
            _audioSpec = NULL;
        }
    } while (false);
    // Eventually update the _state.
    _state = state;
    // Unlock the callback function.
    SDL_UnlockAudio();
}


void Sound::setPlaybackPos(double pos)
{
    assert(pos >= 0 && pos <= 1);
    _pos = (unsigned int)(pos * _len);
}


double Sound::playbackPos() const
{
    return (double)_pos / (double)_len;
}


void Sound::pbCallback(void *user, Uint8 *buf, int size)
{
    Sound *s = static_cast<Sound *>(user);
    // We must pay attention to the fact that our _samples are stored as 16-bit
    // unsigned shorts whereas the callback function deals with bytes!
    size_t bytesToCopy = std::min<size_t>((s->_len - s->_pos) * 2, size);
    if (bytesToCopy) {
        memcpy(buf, s->_samples + s->_pos, bytesToCopy);
        s->_pos += bytesToCopy / 2;
    }
    // Fill the possibly remaining space with silence.
    if (bytesToCopy < (size_t)size)
        memset(buf + bytesToCopy, s->_audioSpec->silence, size - bytesToCopy);
}


} // namespace audio

} // namespace hiwi
