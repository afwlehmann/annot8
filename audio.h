//
// audio.h
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

