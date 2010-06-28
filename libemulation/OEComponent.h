
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

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include <stdio.h>

#define OELog(text) cerr << "libmulator: " << (text) << endl

typedef unsigned char OEUInt8;
typedef char OEInt8;
typedef unsigned short OEUInt16;
typedef short OEInt16;
typedef unsigned int OEUInt32;
typedef int OEInt32;

typedef union
{
#ifndef OE_LSB_FIRST
	struct { OEUInt8 h3, h2, h, l; } b;
	struct { OEInt8 h3, h2, h, l; } sb;
	struct { OEUInt16 h, l; } w;
	struct { OEInt8 h, l; } sw;
#else
	struct { OEUInt8 l, h, h2, h3; } b;
	struct { OEInt8 l, h, h2, h3; } sb;
	struct { OEUInt16 l, h; } w;
	struct { OEInt16 l, h; } sw;
#endif
	OEUInt32 d;
	OEInt32 sd;
} OEPair;

typedef struct
{	
	bool read;
	bool write;
	
	OEUInt32 start;
	OEUInt32 end;
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
	
	virtual bool setMemoryMap(OEComponent *component, const string &value);
	virtual bool getMemoryMap(string &value);
	
	virtual int ioctl(int message, void *data);
	
	virtual OEUInt8 read(OEUInt32 address);
	virtual void write(OEUInt32 address, OEUInt8 value);
	virtual OEUInt16 readw(OEUInt32 address);
	virtual void writew(OEUInt32 address, OEUInt16 value);
	virtual OEUInt32 readd(OEUInt32 address);
	virtual void writed(OEUInt32 address, OEUInt32 value);
	virtual bool read(OEUInt32 address, OEData &value);
	virtual bool write(OEUInt32 address, OEData &value);
	
protected:
	OEObserverMap observerMap;
	
	int getInt(const string &value);
	double getFloat(const string &value);
	string getString(int value);
	string getHex(int value);
	OEData getCharVector(const string &value);
	int getLowerPowerOf2(int value);
	bool getRange(OEMemoryRange &range, const string &value);
	bool getRanges(OEMemoryRanges &ranges, const string &value);
};

#endif
