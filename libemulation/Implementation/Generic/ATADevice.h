
/**
 * libemulation
 * ATA device
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an ATA device
 */

#include "OEComponent.h"

#include "DIATABlockStorage.h"

class ATADevice : public OEComponent
{
public:
    ATADevice();
    ~ATADevice();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEComponent *device;
    
    string diskImagePath;
    
    DIATABlockStorage blockStorage;
    
    bool openDiskImage(string path);
    void closeDiskImage();
};
