//
// Participant.h
// $Id$
//


#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__


#include <string>


namespace hiwi {


struct Participant {
    const int         id;
    const std::string name;
    const std::string firstName;
    const int         number;

    Participant(int aId,
                const std::string &aName,
                const std::string &aFirstName,
                int aNumber) :
        id(aId),
        name(aName),
        firstName(aFirstName),
        number(aNumber)
    {
    }
};


} // namespace hiwi


#endif
