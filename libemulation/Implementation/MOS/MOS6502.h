
/**
 * libemulation
 * MOS6502
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor
 */

#include "OEComponent.h"
#include "ControlBus.h"

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
    
private:
    OEComponent *controlBus;
    OEComponent *memoryBus;
    
    OEUInt8 a;
    OEUInt8 x;
    OEUInt8 y;
    OEUInt8 p;
    OEUnion pc;
    OEUnion sp;
    
    OEInt64 icount;
    
    ControlBusPowerState powerState;
    bool isReset;
    bool isIRQ;
    bool isNMI;
    
    bool isResetStart;
    bool isIRQEnabled;
    
    void execute();
};
