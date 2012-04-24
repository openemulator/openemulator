
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
    
    OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
    
private:
	OEComponent *controlBus;
    OEComponent *floatingBus;
	OEComponent *drive[2];
    
    OEInt phaseControl;
    bool driveOn;
    OEInt driveSel;
    OEInt sequencerMode;
    
    OEInt sequencerState;
    OEChar dataRegister;
    
    OEComponent dummyDrive;
    OEComponent *currentDrive;
    bool timerOn;
    bool reset;
    
    bool driveEnableControl;
    bool writeRequest;
    
    OELong lastCycles;
    
    void setPhaseControl(OEInt index, bool value);
    void updatePhaseControl();
    void setDriveOn(bool value);
    void updateDriveEnableControl();
    void updateDriveEnabled();
    void setDriveSel(OEInt value);
    void updateDriveSel(OEInt value);
    void setSequencerWrite(bool value);
    void setSequencerLoad(bool value);
    void updateWriteRequest();
    OELong getQ3CyclesSinceLastUpdate();
    void updateSequencer(OELong q3Cycles, OEChar value);
};
