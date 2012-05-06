
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

class RDCFFA : public OEComponent
{
public:
    RDCFFA();
    ~RDCFFA();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    string diskImagePath;
    bool forceWriteProtected;
    
    OEComponent *device;
    
    OEComponent *memoryBus;
    
    DIAppleBlockStorage blockStorage;
    
    OEChar ataBuffer[0x200];
    OEInt ataBufferIndex;
    bool ataError;
    OEUnion ataLBA;
    OEChar ataCommand;
    
    bool openDiskImage(string path);
    void closeDiskImage();
};
