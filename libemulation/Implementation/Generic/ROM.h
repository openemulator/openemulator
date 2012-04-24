
/**
 * libemulation
 * ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls read only memory
 */

#include "OEComponent.h"

// Notes:
// * image is the ROM image.

class ROM : public OEComponent
{
public:
    bool setData(string name, OEData *data);
    bool getData(string name, OEData **data);
    bool init();
    
    OEChar read(OEAddress address);
    
private:
    OEData data;
    
    OEChar *datap;
    OEAddress mask;
};
