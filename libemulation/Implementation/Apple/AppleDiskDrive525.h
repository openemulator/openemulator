
/**
 * libemulation
 * Apple 5.25" Disk Drive
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple 5.25" Disk Drive
 */

#include "OEComponent.h"

#include "DeviceInterface.h"

#include "OESound.h"

#include "diskimage.h"

class AppleDiskDrive525 : public OEComponent
{
public:
	AppleDiskDrive525();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	void update();
    void dispose();
    
	bool postMessage(OEComponent *sender, int message, void *data);
	
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
	OEComponent *device;
    OEComponent *controlBus;
    OEComponent *drivePlayer;
    OEComponent *headPlayer;
    
	string diskImagePath;
    string mechanism;
    map<string, OESound>sound;
    
    OEInt phaseControl;
    
    OESInt trackIndex;
    
    DIApple525DiskStorage diskStorage;
    
    DIData track;
    OEChar *trackData;
    OEInt trackDataSize;
    OEInt trackDataIndex;
    
    OEInt zeroCount;
    
    bool isModified;
    
    void updateStepper(OESInt& position, OEInt phaseControl);
    void updateTrack(OEInt value);
    
    void updateSoundSet();
    
    bool openDiskImage(string path);
    bool closeDiskImage();
};
