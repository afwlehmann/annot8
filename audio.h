//
// $Id$
//

#ifndef __AUDIO_H__
#define __AUDIO_H__


#include <stdexcept>


namespace hiwi {

namespace audio {


class AudioException : public std::runtime_error
{
public:
    AudioException(const std::string &what) :
        std::runtime_error(what)
    {}
};


class AudioNotInitializedException : public AudioException
{
public:
    AudioNotInitializedException() :
        AudioException("LibAudio has not been initialized.")
    {}
};


/**
 * Initializes the audio component of the SDL library.
 * @return              true iff the whole initialization went fine
 */
bool initialize();


/**
 * Shuts down the audio component of the SDL library.
 */
void shutdown();


/**
 * Returns whether audio has been initialized.
 */
bool isInitialized();


/**
 * Convenience macro that is mainly used by LibAudio's own functions.
 */
#define ASSERT_AUDIO_INITIALIZED \
    { \
        if (!hiwi::audio::isInitialized()) \
            throw AudioNotInitializedException(); \
    }


} // namespace audio

} // namespace hiwi



#endif

