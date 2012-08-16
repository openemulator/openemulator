
/**
 * libemulation
 * Apple III Disk I/O
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Disk I/O
 */

#ifndef _APPLEIIIDISKIO_H
#define _APPLEIIIDISKIO_H

#include "AppleDiskIIInterfaceCard.h"

class AppleIIIDiskIO : public AppleDiskIIInterfaceCard
{
public:
    AppleIIIDiskIO();
    
    bool setRef(string name, OEComponent *ref);
    
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *systemControl;
    
    bool appleIIMode;
    
    OEInt driveSelect;
    
    void updateSwitches(OEAddress address);
    
    void setDriveSelect(OEInt index, bool value);
    void updateDriveSelect();
    void setDriveSide2(bool value);

    void setENSIO(bool value);
    void setENSEL(bool value);
    void setVideoCharacterWrite(bool value);
    void setVideoScroll(bool value);
};

#endif
