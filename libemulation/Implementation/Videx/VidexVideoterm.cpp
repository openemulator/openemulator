
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include "VidexVideoterm.h"

VidexVideoterm::VidexVideoterm()
{
}

bool VidexVideoterm::setValue(string name, string value)
{
    if (name == "registerSelect")
        registerSelect = getOEInt(value);
    else if (name == "ramBank")
        ramBank = getOEInt(value);
    else if (name == "characterSet1")
        characterSet1 = value;
    else if (name == "characterSet2")
        characterSet2 = value;
    else
        return false;
    
    return true;
}

bool VidexVideoterm::getValue(string name, string &value)
{
    if (name == "registerSelect")
        value = getString(registerSelect);
    else if (name == "characterSet1")
        value = characterSet1;
    else if (name == "characterSet1")
        value = characterSet1;
    else if (name == "characterSet2")
        value = characterSet2;
    else
        return false;
    
    return true;
}

bool VidexVideoterm::init()
{
    return true;
}

// A0 define el register select del 6845
// (A2 A3) >> 2 dan el banco de memoria visible en $CC00
