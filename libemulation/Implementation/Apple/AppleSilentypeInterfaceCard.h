
/**
 * libemulation
 * Apple Silentype Interface Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype Interface Card
 */

#include "OEComponent.h"

class AppleSilentypeInterfaceCard : public OEComponent
{
public:
	AppleSilentypeInterfaceCard();
	
    bool setRef(string name, OEComponent *ref);
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
    
private:
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *slotController;
    OEComponent *memoryCF00;
    OEComponent *memoryMapper;
    OEComponent *printer;
    
    bool shiftClockDisabled;
    
    void setROMEnabled(bool value);
};
