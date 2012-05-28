
/**
 * libemulation
 * R&D CFFA
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA interface card
 */

#include "OEComponent.h"

#include "diskimage.h"

#define ATA_BUFFER_SIZE 0x200

class RDCFFA : public OEComponent
{
public:
    RDCFFA();
    ~RDCFFA();
    
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
    
    DIChar setCSMask;
    DIChar clearCSMask;
    
    DIAppleBlockStorage blockStorage[2];
    
    bool ataDrive;
    OEChar ataCommand;
    OEChar ataStatus;
    OEUnion ataLBA;
    OEChar ataBuffer[ATA_BUFFER_SIZE];
    OEInt ataBufferIndex;
    OEChar ataDataHigh;
    
    bool openDiskImage(string path);
    void closeDiskImage();
};
