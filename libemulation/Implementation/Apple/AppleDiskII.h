
/**
 * libemulation
 * Apple Disk II
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "OEComponent.h"

#include "DeviceInterface.h"

#include "OESound.h"

class AppleDiskII : public OEComponent
{
public:
	AppleDiskII();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	void update();
    
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	OEComponent *device;
    OEComponent *drivePlayer;
    OEComponent *headPlayer;
    
	string diskImage;
	bool forceWriteProtected;
    string mechanism;
    map<string, OESound>sound;
    
    OEUInt32 phaseControl;
    OEInt32 trackIndex;
    bool isWriteProtected;
    
    void updateSound();
    void setPhaseControl(OEUInt32 value);
};
