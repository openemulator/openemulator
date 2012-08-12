
/**
 * libemulation
 * Apple III Beeper
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III Beeper
 */

#include "OEComponent.h"

class AppleIIIBeeper : public OEComponent
{
public:
    AppleIIIBeeper();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *audioOut;
    
    OEInt count;
    
    void scheduleNextTimer(OESLong cycles);
};
