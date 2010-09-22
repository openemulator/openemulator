
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
	virtual bool setValue(const string &name, string &value);
	virtual bool getValue(const string &name, string &value);
	virtual bool setComponent(const string &name, OEComponent *component);
	virtual bool setData(const string &name, OEData *data);
	virtual bool getData(const string &name, OEData **data);
	
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
	virtual int read(OEUInt32 address);
	virtual void write(OEUInt32 address, int value);
	virtual int read16(OEUInt32 address);
	virtual void write16(OEUInt32 address, int value);
	virtual int read32(OEUInt32 address);
	virtual void write32(OEUInt32 address, int value);
	virtual bool readBlock(OEUInt32 address, OEData *value);
	virtual bool writeBlock(OEUInt32 address, OEData *value);
	
	// Debugging
	virtual bool sendDebugCommand(const string &command);
	
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
