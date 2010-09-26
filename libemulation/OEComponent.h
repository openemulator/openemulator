
/**
 * libemulation
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component definition
 */

#ifndef _OECOMPONENT_H
#define _OECOMPONENT_H

#include "OETypes.h"

class OEComponent;
typedef vector<OEComponent *> OEComponents;
typedef map<int, OEComponents> OEObservers;
typedef map<int, OEComponents> OEDelegates;

class OEComponent
{
public:
	OEComponent();
	virtual ~OEComponent();
	
	// Configuration
	virtual bool setValue(string name, string value);
	virtual bool getValue(string name, string &value);
	virtual bool setComponent(string name, OEComponent *component);
	virtual bool setData(string name, OEData *data);
	virtual bool getData(string name, OEData **data);
	
	virtual bool init();
	
	// Notifications
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	virtual void notify(OEComponent *component, int notification, void *data);
	
	// Events and responder chain
	bool addDelegate(OEComponent *component, int event);
	bool removeDelegate(OEComponent *component, int event);
	virtual bool postEvent(OEComponent *component, int event, void *data);
	
	// Memory access
	virtual OEUInt8 read(OEAddress address);
	virtual void write(OEAddress address, OEUInt8 value);
	virtual OEUInt16 read16(OEAddress address);
	virtual void write16(OEAddress address, OEUInt16 value);
	virtual OEUInt32 read32(OEAddress address);
	virtual void write32(OEAddress address, OEUInt32 value);
	virtual bool readBlock(OEAddress address, OEData *value);
	virtual bool writeBlock(OEAddress address, OEData *value);
	
	// Debugging
	virtual bool sendDebugCommand(string command);
	
protected:
	OEObservers observers;
	OEDelegates delegates;
	
	// Helpers
	int getInt(const string &value);
	double getFloat(const string &value);
	string getString(int value);
	string getHex(int value);
	OEData getCharVector(const string &value);
	int getNextPowerOf2(int value);
};

#endif
