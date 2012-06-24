
/**
 * libemulation
 * ATA Controller
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a generic ATA Controller
 */

#include "OEComponent.h"

#include "diskimage.h"

#define ATA_BUFFER_SIZE 0x200

class ATAController : public OEComponent
{
public:
    ATAController();
    ~ATAController();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    string diskImagePath;
    bool forceWriteProtected;
    
    OEComponent *device;
    
    DIAppleBlockStorage blockStorage[2];
    
    bool drive;
    OEUnion lba;
    OEChar sectorCount;
    OEChar feature;
    OEChar status;
    OEChar command;
    OEChar buffer[ATA_BUFFER_SIZE];
    OEInt bufferIndex;
    
    bool byteMode;
    
    bool openDiskImage(string path);
    void closeDiskImage();
};
