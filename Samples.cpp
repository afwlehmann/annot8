//
// Samples.h
// $Id$
//


#include "Samples.h"
#include <stdexcept>
#include <SDL/SDL_sound.h>


namespace hiwi {

namespace audio {


Samples::Samples(const std::string &fileName) :
    _ss(0),
    _numSamples(-1)
{
    // Define the desired format. However, SDL_sound seems to ignore this ;-(
    Sound_AudioInfo ainfo;
    ainfo.format = AUDIO_S16SYS;
    ainfo.channels = 1;
    ainfo.rate = 44100;

    // Load the samples.
    _ss = Sound_NewSampleFromFile(fileName.c_str(), &ainfo, 4096);
    if (!_ss || _ss->flags & SOUND_SAMPLEFLAG_ERROR)
        throw std::runtime_error(SDL_GetError());

    // Assure the correct format. Note that the rate doesn't matter at this
    // point.
// FIXME: Commented this out because it seems as if the `actual' field of SDL's
//        Sound_Sample doesn't represent the actual format after conversion,
//        but the actual file format of the original data instead.
//        At the time being, the PlaybackThread assumes the buffered data to be
//        AUDIO_S16SYS, 1 channel and 44KHz.
//    if (_ss->actual.channels != 1 || _ss->actual.format != AUDIO_S16SYS)
//    {
//        Sound_FreeSample(_ss);
//        _ss = 0;
//        throw std::runtime_error("SDL_sound couldn't deliver the neccessary "
//                                 "audio format.");
//    }

    // SDL limits the sample frequency from 11kHz to 44kHz.
    assert(_ss->actual.rate >= MinSampleFreq && _ss->actual.rate <= MaxSampleFreq);

    // Then try to decode the whole file.
    Sound_DecodeAll(_ss);
    if (!(_ss->flags & SOUND_SAMPLEFLAG_EOF)) {
        Sound_FreeSample(_ss);
        _ss = 0;
        throw std::runtime_error("Error while decoding the sound samples.");
    }

    // Allocate a suitable array of floats and store a mapped copy of the
    // samples in that array.
    _numSamples = _ss->buffer_size / 2;
    _samplesAsFloat = new float[_numSamples];
    for (size_t i = 0; i < _numSamples; i++)
        _samplesAsFloat[i] = (2 * ((short *)(_ss->buffer))[i] + 1) / 65535.0;
}


Samples::~Samples()
{
    // Now free the rest of the allocated memory.
    if (_samplesAsFloat) {
        delete[] _samplesAsFloat;
        _samplesAsFloat = 0;
    }
    if (_ss) {
        Sound_FreeSample(_ss);
        _ss = 0;
    }
}


} // namespace audio

} // namespace hiwi
