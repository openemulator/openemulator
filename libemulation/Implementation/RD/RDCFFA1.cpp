
/**
 * libemulation
 * R&D CFFA1
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA1 interface card
 */

#include "RDCFFA1.h"

RDCFFA1::RDCFFA1()
{
    controlBus = NULL;
    memoryBus = NULL;
}

bool RDCFFA1::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
        controlBus = ref;
    else if (name == "memoryBus")
        memoryBus = ref;
    else
        return false;
    
    return true;
}
