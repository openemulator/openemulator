
/**
 * libemulation
 * Apple Graphics Tablet Interface Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet Interface Card
 */

#include "OEComponent.h"

class AppleGraphicsTabletInterfaceCard : public OEComponent
{
public:
	AppleGraphicsTabletInterfaceCard();
	
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
    
private:
    OEComponent *graphicsTablet;
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *memory;
    OEComponent *audioCodec;
    
    bool proximity;
    float x;
    float y;
    bool button;
    
    OEInt count;
    
    bool timerEnabled;
    OEInt timerCount;
    OELong timerCycles;
    
    void setTimer(float value);
    void updateCount();
};
