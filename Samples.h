//
// Samples.h
// $Id$
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


namespace hiwi {

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

} // namespace hiwi


#endif
