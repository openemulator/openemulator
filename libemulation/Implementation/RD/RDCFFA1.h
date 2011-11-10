
/**
 * libemulation
 * R&D CFFA1
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA1 interface card
 */

#include "OEComponent.h"

class RDCFFA1 : public OEComponent
{
public:
    RDCFFA1();
    ~RDCFFA1();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *device;
    
    OEComponent *ram;
    OEComponent *rom;
    
    OEComponent *controlBus;
    OEComponent *mmu;
    
    string diskImage;
    FILE *diskImageFP;
    bool forceWriteProtected;
    
    OEUInt8 ataBuffer[0x200];
    OEUInt32 ataBufferIndex;
    bool ataError;
    OEUnion ataLBA;
    OEUInt8 ataCommand;
    
    void mapMMU(int message);
    bool openDiskImage(string filename);
    void closeDiskImage();
};
