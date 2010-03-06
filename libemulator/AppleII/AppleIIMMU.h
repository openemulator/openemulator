
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II D000-FFFF range.
 */

#include "OEComponent.h"

enum
{
	APPLEIIMMU_DISABLE_ROM = OEIOCTL_USER,
	APPLEIIMMU_ENABLE_ROM,
};

class AppleIIMMU : public OEComponent
{
public:
	AppleIIMMU();
	~AppleIIMMU();
	
	int ioctl(int message, void *data);
	void onNotification(OEComponent *component, int message, void *data);
	
private:
	int offset;
	
	OEComponent *hostSystem;
	OEComponent *floatingBus;
	OEComponent *memory;
	
	OEComponent *romD0;
	OEComponent *romD8;
	OEComponent *romE0;
	OEComponent *romE8;
	OEComponent *romF0;
	OEComponent *romF8;
	
	void mapComponent(OEComponent *component);
	void mapFloatingBus();
};
