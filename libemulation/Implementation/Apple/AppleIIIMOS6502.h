
/**
 * libemulation
 * Apple III MOS6502
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates an Apple III MOS6502 microprocessor
 */

#ifndef _APPLEIIIMOS6502_H
#define _APPLEIIIMOS6502_H

#include "MOS6502.h"

class AppleIIIMOS6502 : public MOS6502
{
public:
    AppleIIIMOS6502();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEComponent *extendedMemoryBus;
    OEComponent *systemControl;
    
    bool extendedMemoryEnabled;
    OEAddress extendedPageAddress;
    OEInt extendedMemoryBank;
    
    void execute();
    void setExtendedMemoryBank(OEInt value);
};

#endif
