
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
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *cpu;
    OEComponent *controlBus;
    OEComponent *zeroPageSwitcher;
    OEComponent *bankSwitcher;
    OEComponent *extendedZeroPageSwitcher;
    OEComponent *extendedBankSwitcher;
    OEComponent *ram;
    OEComponent *keyboard;
    OEComponent *video;
    OEComponent *rtc;
    OEComponent *silentype;
    OEComponent *dVIA;
    OEComponent *eVIA;
    OEComponent *dac;
    
    OEChar ramMask;
    
    OEChar environment;
    OEChar zeroPage;
    OEChar ramBank;
    OEChar sound;
    
    OEChar extendedRAMBank;
    
    bool monitorRequested;
    
    void setEnvironment(OEChar value);
    void setZeroPage(OEChar value);
    void setRAMBank(OEChar value);
    void setExtendedRAMBank(OEChar value);
    void setSound(OEChar value);
    
    void updateEnvironment();
    void updateZeroPage();
    void updateAltStack();
    void updateRAMBank();
    void updateExtendedRAMBank();
};

#endif
