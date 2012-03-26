
/**
 * libemulation
 * Apple Disk II Interface Card
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple Disk II interface card
 */

#include "OEComponent.h"

class AppleDiskIIInterfaceCard : public OEComponent
{
public:
	AppleDiskIIInterfaceCard();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool init();
	void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
    
private:
	OEComponent *controlBus;
	OEComponent *drive[2];
    
    OEUInt32 phase;
    OEUInt32 driveSel;
    bool driveOn;
    
    OEComponent *currentDrive;
    bool reset;
    bool timerOn;
    
    OEComponent dummyDrive;
    
    void setPhase(OEUInt32 index, bool value);
    void setDriveOn(bool value);
    void updateDriveOn();
    void setDriveSel(OEUInt32 value);
    void updateDriveSel(OEUInt32 value);
    void setSequencer(bool shift, bool value);
};
