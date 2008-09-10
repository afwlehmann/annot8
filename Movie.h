//
// Movie.h
// $Id$
//


#ifndef __MOVIE_H__
#define __MOVIE_H__


#include <string>


namespace hiwi {


struct Movie {
    const std::string prefix;
    const std::string suffix;
    const int         firstFrame;
    const int         lastFrame;
    const int         numDigits;
    const std::string description;

    Movie(const std::string &aPrefix, const std::string &aSuffix,
          int aFirstFrame, int aLastFrame, int aNumDigits,
          const std::string &aDescription) :
        prefix(aPrefix),
        suffix(aSuffix),
        firstFrame(aFirstFrame),
        lastFrame(aLastFrame),
        numDigits(aNumDigits),
        description(aDescription)
    {
    }
};


} // namespace hiwi


#endif
