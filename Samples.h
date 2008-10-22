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


#ifndef __SAMPLES_H__
#define __SAMPLES_H__


// SDL behaves terrible by means of defining a `main'-macro. Since we don't
// want everybody who includes this header to end up with this `main'-macro,
// we circumvent the problem as follows:
#ifdef main
#  include <SDL/SDL_sound.h>
#else
#  include <SDL/SDL_sound.h>
#  undef main
#endif

#include <string>


namespace annot8 {

namespace audio {


struct Samples
{
    // SDL supports only a limited range of sample frequencies for playback,
    // hence:
    static const unsigned int MinSampleFreq = 11025;
    static const unsigned int MaxSampleFreq = 44100;


    Sound_Sample    *ss;
    float           *samplesAsFloat;
    size_t          numSamples;


    /**
     * Constructs a new instance of Samples for the specified filename, i.e.
     * loads the associated samples and stores them as both raw and floating
     * point data.
     */
    Samples(const std::string &fileName);


    /**
     * Destructs an instance of Samples and frees all allocated memory.
     */
    virtual ~Samples();
};


} // namespace audio

} // namespace annot8


#endif
