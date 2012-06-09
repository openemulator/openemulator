
/**
 * libemulator
 * Apple II Slot Controller
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AppleIISlotController : public OEComponent
{
public:
    AppleIISlotController();
    
	bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *memory;
    OEComponent *memoryBus;
	
	bool en;
    
    MemoryMapsConf conf;
    MemoryMapsRef ref;
    
    MemoryMaps memoryMaps;
    
    void enableSlotExpansion(bool value);
    void updateSlotExpansion(bool value);
};
