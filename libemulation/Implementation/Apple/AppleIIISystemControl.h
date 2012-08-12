
/**
 * libemulation
 * Apple III System Control
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III system control
 */

#ifndef _APPLEIIISYSTEMCONTROL_H
#define _APPLEIIISYSTEMCONTROL_H

#include "OEComponent.h"

class AppleIIISystemControl : public OEComponent
{
public:
    AppleIIISystemControl();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *cpu;
    OEComponent *controlBus;
    OEComponent *memoryBus;
    OEComponent *extendedMemoryBus;
    OEComponent *memoryAddressDecoder;
    OEComponent *memory;
    OEComponent *video;
    OEComponent *dVIA;
    OEComponent *eVIA;
    OEComponent *dac;
    
    OEChar environment;
    OEChar zeroPage;
    OEChar ramBank;
    OEChar dacOutput;
    
    bool monitorRequested;
    
    void setEnvironment(OEChar value);
    void setZeroPage(OEChar value);
    void setRAMBank(OEChar value);
    void setDACOutput(OEChar value);
    
    void updateNormalStack();
};

#endif
