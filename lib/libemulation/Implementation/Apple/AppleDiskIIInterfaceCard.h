
/**
 * libemulation
 * Apple Disk II Interface Card
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple Disk II interface card
 */

#ifndef _APPLEDISKIIINTERFACECARD_H
#define _APPLEDISKIIINTERFACECARD_H

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
    
protected:
    OEComponent *floatingBus;
    
    OEChar dataRegister;
    
    bool driveEnableControl;
    
    void setPhaseControl(OEInt index, bool value);
    void setDriveOn(bool value);
    void selectDrive(OEInt value);
    void setSequencerWrite(bool value);
    void setSequencerLoad(bool value);
    void updateSequencer();
    
private:
	OEComponent *controlBus;
	OEComponent *drive[5];
    
    OEInt phaseControl;
    bool driveOn;
    OEInt driveSel;
    OEInt sequencerMode;
    
    bool sequencerState;
    
    OEComponent dummyDrive;
    OEComponent *currentDrive;
    bool timerOn;
    bool reset;
    
    OELong lastCycles;
    
    void updateSwitches(OEAddress address);
    void updatePhaseControl();
    void updateDriveEnableControl();
    void updateDriveEnabled();
    void updateDriveSelection(OEInt value);
};

#endif
