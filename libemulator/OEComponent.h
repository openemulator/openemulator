
/**
 * libemulator
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component definition
 */

#ifndef _OECOMPONENT_H
#define _OECOMPONENT_H

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include <stdio.h>

#define OELog(text) cerr << "OEEmulator: " << (text) << endl

typedef unsigned char UInt8;
typedef char Int8;
typedef unsigned short UInt16;
typedef short Int16;
typedef unsigned int UInt32;
typedef int Int32;

typedef union
{
#ifndef OE_LSB_FIRST
	struct { UInt8 h3, h2, h, l; } b;
	struct { Int8 h3, h2, h, l; } sb;
	struct { UInt16 h, l; } w;
	struct { Int8 h, l; } sw;
#else
	struct { UInt8 l, h, h2, h3; } b;
	struct { Int8 l, h, h2, h3; } sb;
	struct { UInt16 l, h; } w;
	struct { Int16 l, h; } sw;
#endif
	UInt32 d;
	Int32 sd;
} OEPair;

typedef struct
{	
	bool read;
	bool write;
	
	UInt32 start;
	UInt32 end;
} OEMemoryRange;

using namespace std;

class OEComponent;

typedef vector<char> OEData;
typedef vector<OEComponent *> OEObservers;
typedef map<int, OEObservers> OEObserverMap;
typedef vector<OEMemoryRange> OEMemoryRanges;

class OEComponent
{
public:
	OEComponent();
	virtual ~OEComponent();
	
	virtual bool setProperty(const string &name, const string &value);
	virtual bool getProperty(const string &name, string &value);
	virtual bool setData(const string &name, const OEData &data);
	virtual bool getData(const string &name, OEData &data);
	virtual bool setResource(const string &name, const OEData &data);
	virtual bool connect(const string &name, OEComponent *component);
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	void postNotification(int notification, void *data);
	virtual void notify(int notification, OEComponent *component, void *data);
	
	virtual bool setMemoryMap(OEComponent *component, const string &ranges);
	virtual bool getMemoryMap(string &ranges);
	
	bool assertInterrupt(int id);
	bool releaseInterrupt(int id);
	
	virtual int ioctl(int message, void *data);
	
	virtual int read(int address);
	virtual void write(int address, int value);
	
protected:
	OEObserverMap observerMap;
	
	int getInt(const string &value);
	double getFloat(const string &value);
	string getString(int value);
	string getHex(int value);
	OEData getCharVector(const string &value);
	int getLowerPowerOf2(int value);
	OEMemoryRanges getRanges(const string &ranges);
};

#endif
