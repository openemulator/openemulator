
/**
 * libemulation
 * Apple III VIA Control
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III VIA control
 */

#ifndef _APPLEIIIVIACONTROL_H
#define _APPLEIIIVIACONTROL_H

#include "OEComponent.h"

class AppleIIIVIAControl : public OEComponent
{
public:
    AppleIIIVIAControl();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *ramBanking;
    OEComponent *dacAudioCodec;
    
    OEChar environment;
    OEChar zeroPage;
};

#endif
