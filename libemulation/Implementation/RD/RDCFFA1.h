
/**
 * libemulation
 * R&D CFFA1
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA1 interface card
 */

#include "OEComponent.h"

class RDCFFA1 : public OEComponent
{
public:
    RDCFFA1();
    
    bool setRef(string name, OEComponent *ref);
    
private:
    OEComponent *controlBus;
    OEComponent *memoryBus;
};
