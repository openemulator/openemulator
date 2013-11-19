
/**
 * libemulation
 * MOS6522
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6522 VIA (Versatile Interface Adaptor)
 */

#ifndef _MOS6522_H
#define _MOS6522_H

#include "OEComponent.h"

// Messages
typedef enum
{
    MOS6522_GET_PA,
	MOS6522_SET_CA1,
	MOS6522_SET_CA2,
    
    MOS6522_GET_PB,
	MOS6522_SET_CB1,
	MOS6522_SET_CB2,
} MOS6522Message;

class MOS6522 : public OEComponent
{
public:
    MOS6522();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *controlBus;
    OEComponent *portA;
    OEComponent *portB;
    
    OEAddress addressA;
    OEChar ddrA;
    OEChar dataA;
	bool ca1;
	bool ca2;
    
    OEAddress addressB;
    OEChar ddrB;
    OEChar dataB;
	bool cb1;
	bool cb2;
    
    OEChar shift;
    
    OEChar auxControl;
    OEChar peripheralControl;
    
    OEChar interruptFlags;
    OEChar interruptEnable;
    
    void updateIRQ();
};

#endif
