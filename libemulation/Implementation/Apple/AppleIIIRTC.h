
/**
 * libemulation
 * Apple III RTC
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III RTC
 */

#ifndef _APPLEIIIRTC_H
#define _APPLEIIIRTC_H

#include "OEComponent.h"

class AppleIIIRTC : public OEComponent
{
public:
    AppleIIIRTC();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *systemControl;
    OEComponent *mm58167;
    
    OEChar zeroPage;
};

#endif
