
/**
 * libemulation
 * OEComponent
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the base OpenEmulator component
 */

#ifndef _OECOMPONENT_H
#define _OECOMPONENT_H

#include "OECommon.h"

class OEComponent;

typedef vector<OEComponent *> OEComponents;
typedef map<int, OEComponents> OEObservers;

class OEComponent
{
public:
    virtual ~OEComponent();
    
    // Configuration
    virtual bool setValue(string name, string value);
    virtual bool getValue(string name, string& value);
    virtual bool setRef(string name, OEComponent *ref);
    virtual bool setData(string name, OEData *data);
    virtual bool getData(string name, OEData **data);
    virtual bool init();
    virtual void update();
    
    // Messaging
    virtual bool postMessage(OEComponent *sender, int message, void *data);
    
    // Notification
    virtual void notify(OEComponent *sender, int notification, void *data);
    virtual bool addObserver(OEComponent *observer, int notification);
    virtual bool removeObserver(OEComponent *observer, int notification);
    
    // Memory access
    virtual OEUInt8 read(OEAddress address);
    virtual void write(OEAddress address, OEUInt8 value);
    virtual OEUInt16 read16(OEAddress address);
    virtual void write16(OEAddress address, OEUInt16 value);
    virtual OEUInt32 read32(OEAddress address);
    virtual void write32(OEAddress address, OEUInt32 value);
    virtual OEUInt64 read64(OEAddress address);
    virtual void write64(OEAddress address, OEUInt64 value);
    
protected:
    OEObservers observers;
};

#endif
