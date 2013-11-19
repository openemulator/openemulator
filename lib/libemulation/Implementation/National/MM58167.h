
/**
 * libemulation
 * MM58167
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MM58167 real time clock
 */

#ifndef MM58167_H
#define MM58167_H

#include "OEComponent.h"

class MM58167 : public OEComponent
{
public:
    MM58167();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *audio;
    
    OEChar getBCD(OEChar value);
    
    OEInt deltaSec;
    OEInt deltaUSec;
    
    OEChar interruptFlags;
    OEChar interruptControl;
    
    OEData ram;
    OEChar *ramp;
};

#endif
