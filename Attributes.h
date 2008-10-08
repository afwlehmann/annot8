//
// Attributes.h
// $Id$
//


#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__


namespace hiwi {


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


} // namespace hiwi


#endif
