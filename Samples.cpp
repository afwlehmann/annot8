//
// Samples.h
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


#include "Samples.h"
#include "PlaybackThread.h"
#include <stdexcept>
#include <SDL/SDL_sound.h>


namespace hiwi {

namespace audio {


Samples::Samples(const std::string &fileName) :
    ss(0),
    numSamples(-1)
{
    // Define the desired format. However, SDL_sound seems to ignore this ;-(
    Sound_AudioInfo ainfo;
    ainfo.format = AUDIO_S16SYS;
    ainfo.channels = 1;
    ainfo.rate = PlaybackThread::DefaultFrequency;

    // Load the samples.
    // Note: If the initial buffer size's magnitude is chosen less than 1L<<24,
    //       Sound_DecodeAll needs forever on win32 platforms.
    ss = Sound_NewSampleFromFile(fileName.c_str(), &ainfo, 1L<<24);
    if (!ss || ss->flags & SOUND_SAMPLEFLAG_ERROR)
        throw std::runtime_error(SDL_GetError());

// XXX: Commented this out because it seems as if the `actual' field of SDL's
//      Sound_Sample doesn't represent the actual format after conversion, but
//      the actual file format of the original data instead.
//      At the time being, the PlaybackThread assumes the buffered data to be
//      AUDIO_S16SYS, 1 channel and at `PlaybackThread::DefaultFrequency'.
//    // Assure the correct format. Note that the rate doesn't matter at this
//    // point.
//    if (ss->actual.channels != 1 || ss->actual.format != AUDIO_S16SYS)
//    {
//        Sound_FreeSample(ss);
//        ss = 0;
//        throw std::runtime_error("SDL_sound couldn't deliver the neccessary "
//                                 "audio format.");
//    }

    // SDL limits the sample frequency from 11kHz to 44kHz.
    if (ss->actual.rate < MinSampleFreq || ss->actual.rate > MaxSampleFreq) {
        Sound_FreeSample(ss);
        ss = 0;
        throw std::runtime_error("Invalid sample frequency.");
    }

    // Then try to decode the whole file.
    Sound_DecodeAll(ss);
    if (!(ss->flags & SOUND_SAMPLEFLAG_EOF)) {
        Sound_FreeSample(ss);
        ss = 0;
        throw std::runtime_error("Error while decoding the sound samples.");
    }

    // Allocate a suitable array of floats and store a mapped copy of the
    // samples in that array.
    numSamples = ss->buffer_size / 2;
    samplesAsFloat = new float[numSamples];
    for (size_t i = 0; i < numSamples; i++)
        samplesAsFloat[i] = (2 * ((short *)(ss->buffer))[i] + 1) / 65535.0;
}


Samples::~Samples()
{
    // First get rid of the samples in floating-point format.
    if (samplesAsFloat) {
        delete[] samplesAsFloat;
        samplesAsFloat = 0;
    }
    // Then dispose of the sdl-related portion of the samples.
    if (ss) {
        Sound_FreeSample(ss);
        ss = 0;
    }
}


} // namespace audio

} // namespace hiwi
