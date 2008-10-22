//
// Participant.h
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


#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__


#include <string>


namespace annot8 {


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


} // namespace annot8


#endif
