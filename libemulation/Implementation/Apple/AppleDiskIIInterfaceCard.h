
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
    OEComponent *floatingBus;
	OEComponent *drive[2];
    
    OEUInt32 phaseControl;
    bool driveOn;
    OEUInt32 driveSel;
    OEUInt32 sequencerMode;
    
    OEUInt32 sequencerState;
    OEUInt8 dataRegister;
    
    OEComponent dummyDrive;
    OEComponent *currentDrive;
    bool timerOn;
    bool reset;
    
    bool driveEnableControl;
    bool writeRequest;
    
    OEUInt64 lastCycles;
    
    void setPhaseControl(OEUInt32 index, bool value);
    void updatePhaseControl();
    void setDriveOn(bool value);
    void updateDriveEnableControl();
    void updateDriveEnabled();
    void setDriveSel(OEUInt32 value);
    void updateDriveSel(OEUInt32 value);
    void setSequencerWrite(bool value);
    void setSequencerLoad(bool value);
    void updateWriteRequest();
    OEUInt64 getQ3CyclesSinceLastUpdate();
    void updateSequencer(OEUInt64 q3Cycles, OEUInt8 value);
};
