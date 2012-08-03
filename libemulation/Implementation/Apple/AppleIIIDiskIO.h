
/**
 * libemulation
 * Apple III Disk I/O
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Disk I/O
 */

#include "OEComponent.h"
#include "AppleDiskIIInterfaceCard.h"

class AppleIIIDiskIO : public AppleDiskIIInterfaceCard
{
public:
    AppleIIIDiskIO();
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEInt driveSelect;
    
    void setDriveSelect(OEInt index, bool value);
    void setDriveSide2(bool value);
};
