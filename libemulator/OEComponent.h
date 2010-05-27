
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

#include <stdio.h>

#define OELog(...) fprintf(stderr, __VA_ARGS__)

typedef unsigned char UINT8;
typedef char INT8;
typedef unsigned short UINT16;
typedef short INT16;
typedef unsigned int UINT32;
typedef int INT32;

typedef union
{
#ifndef OE_LSB_FIRST
	struct { UINT8 h3, h2, h, l; } b;
	struct { INT8 h3, h2, h, l; } sb;
	struct { UINT16 h, l; } w;
	struct { INT8 h, l; } sw;
#else
	struct { UINT8 l, h, h2, h3; } b;
	struct { INT8 l, h, h2, h3; } sb;
	struct { UINT16 l, h; } w;
	struct { INT16 l, h; } sw;
#endif
	UINT32 d;
	INT32 sd;
} OEPair;

using namespace std;

class OEComponent;

typedef vector<char> OEData;
typedef vector<string> OEMemoryRange;
typedef vector<OEComponent *> OEObservers;
typedef map<int, OEObservers> OEObserverMap;

class OEComponent
{
public:
	OEComponent();
	virtual ~OEComponent();
	
	virtual bool setProperty(string name, string &value);
	virtual bool getProperty(string name, string &value);
	virtual bool setData(string name, OEData &data);
	virtual bool getData(string name, OEData &data);
	virtual bool setResource(string name, OEData &data);
	virtual bool connect(string name, OEComponent *component);
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	virtual void notify(int notification, OEComponent *component, void *data);
	
	virtual bool setMemoryMap(OEComponent *component, OEMemoryRange &range);
	virtual bool getMemoryMap(OEMemoryRange &range);
	
	virtual bool assertInterrupt(int id);
	virtual bool releaseInterrupt(int id);
	
	virtual int ioctl(int message, void *data);
	
	virtual int read(int address);
	virtual void write(int address, int value);
	
protected:
	OEObserverMap observers;
	
	void postNotification(int notification, void *data);
	
	int getInt(string value);
	double getFloat(string value);
	string getString(int value);
	string getHex(int value);
	vector<char> getCharVector(string value);
	int getLowerPowerOf2(int value);
	string getRange(int start, int end);
};

#endif
