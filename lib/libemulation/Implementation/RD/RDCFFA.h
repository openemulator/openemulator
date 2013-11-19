
/**
 * libemulation
 * R&D CFFA ATA Interface
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an R&D CFFA ATA Interface
 */

#include "OEComponent.h"

class RDCFFA : public OEComponent
{
public:
    RDCFFA();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
private:
    OEComponent *ataController;
    
    bool csMask;
    OEShort ataData;
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
};
