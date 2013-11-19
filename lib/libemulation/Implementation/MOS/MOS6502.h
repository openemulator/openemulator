
/**
 * libemulation
 * MOS6502
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor
 */

#ifndef _MOS6502_H
#define _MOS6502_H

#include "OEComponent.h"
#include "ControlBusInterface.h"

class MOS6502 : public OEComponent
{
public:
    MOS6502();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
protected:
    OEChar a;
    OEChar x;
    OEChar y;
    OEChar p;
    OEUnion pc;
    OEUnion sp;
    
    OEComponent *controlBus;
    OEComponent *memoryBus;
    
    OESLong icount;
    
    ControlBusPowerState powerState;
    
    bool isReset;
    bool isResetTransition;
    bool isIRQ;
    bool isIRQEnabled;
    bool isNMITransition;
    
    bool isSpecialCondition;
    
    void initCPU();
    void updateSpecialCondition();
    virtual void execute();
};

#endif
