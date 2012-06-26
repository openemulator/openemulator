
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
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    OEShort read16(OEAddress address);
    void write16(OEAddress address, OEShort value);
    
private:
    OEComponent *drive[2];
    
    DIATABlockStorage *blockStorage;
    DIATABlockStorage dummyBlockStorage;
    
    OEChar feature;
    OEChar status;
    OEChar command;
    
    OEUnion lba;
    OEChar sectorCount;
    
    OEChar buffer[ATA_BUFFER_SIZE];
    OEInt bufferIndex;
    
    bool driveSel;
    OEInt addressMode;
    bool pioByteMode;
    
    void selectDrive(OEInt value);
    void setATAString(char *dest, const char *src, OEInt size);
};
