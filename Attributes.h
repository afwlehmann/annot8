//
// Attributes.h
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


#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__


namespace annot8 {


struct Attributes {
    bool speaking;
    bool laughing;

    Attributes() :
        speaking(false),
        laughing(false)
    {
    }
    
    Attributes(bool aSpeaking, bool aLaughing) :
        speaking(aSpeaking),
        laughing(aLaughing)
    {
    }
};


} // namespace annot8


#endif
