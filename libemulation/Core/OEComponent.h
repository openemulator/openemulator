
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
	virtual ~OEComponent();
	
	// Configuration
	virtual bool setValue(string name, string value);
	virtual bool getValue(string name, string& value);
	virtual bool setRef(string name, OEComponent *ref);
	virtual bool setData(string name, OEData *data);
	virtual bool getData(string name, OEData **data);
	virtual bool init();
	virtual void update();
	
	// Messaging and delegation
	virtual bool postMessage(OEComponent *sender, int message, void *data);
	bool addDelegate(OEComponent *delegate, int message);
	bool removeDelegate(OEComponent *delegate, int message);
	void setDelegate(OEComponent *oldDelegate,
					 OEComponent *newDelegate,
					 int message);
	
	// Notification
	virtual void notify(OEComponent *sender, int notification, void *data);
	bool addObserver(OEComponent *observer, int notification);
	bool removeObserver(OEComponent *observer, int notification);
	void setObserver(OEComponent *oldObserver,
					 OEComponent *newObserver,
					 int notification);
	
	// Memory access
	virtual OEUInt8 read(OEAddress address);
	virtual void write(OEAddress address, OEUInt8 value);
	virtual OEUInt16 read16(OEAddress address);
	virtual void write16(OEAddress address, OEUInt16 value);
	virtual OEUInt32 read32(OEAddress address);
	virtual void write32(OEAddress address, OEUInt32 value);
	virtual OEUInt64 read64(OEAddress address);
	virtual void write64(OEAddress address, OEUInt64 value);
	virtual int readBlock(OEAddress address, OEData *value);
	virtual int writeBlock(OEAddress address, OEData *value);
	
protected:
	// Helpers
	void logMessage(string message);
	
	int getInt(const string& value);
	double getFloat(const string& value);
	OEData getCharVector(const string& value);
	
	string getString(int value);
	string getString(float value);
	string getHexString(int value);
	
	int getNextPowerOf2(int value);
	
private:
	OEDelegates delegates;
	OEObservers observers;
};

#endif
