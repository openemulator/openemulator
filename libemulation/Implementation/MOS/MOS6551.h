
/**
 * libemulation
 * MOS6551
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6551 ACIA
 */

#ifndef _MOS6551_H
#define _MOS6551_H

#include "OEComponent.h"

class MOS6551 : public OEComponent
{
public:
    MOS6551();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *controlBus;
    
    OEChar dataRegister;
    OEChar statusRegister;
    OEChar commandRegister;
    OEChar controlRegister;
    
    void initACIA();
};

#endif
