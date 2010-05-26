
/**
 * libemulator
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component type
 */

#ifndef _OECOMPONENT_H
#define _OECOMPONENT_H

#include <string>
#include <vector>

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

// Messages
enum {
	OE_SET_PROPERTY,
	OE_GET_PROPERTY,
	OE_SET_DATA,
	OE_GET_DATA,
	OE_SET_RESOURCE,
	OE_CONNECT,
	OE_NOTIFY,
	OE_SET_MEMORYMAP,
	OE_GET_MEMORYMAP,
	OE_ASSERT_IRQ,
	OE_RELEASE_IRQ,
	OE_USER,
};

using namespace std;

class OEComponent
{
public:
	OEComponent();
	virtual ~OEComponent();
	
	virtual int ioctl(int message, void *data);
	virtual int read(int address);
	virtual void write(int address, int value);
	
protected:
	void addObserver(vector<OEComponent *> &o,
					 OEComponent *component);
	void removeObserver(vector<OEComponent *> &o,
						OEComponent *component);
	void postNotification(int id,
						  vector<OEComponent *> &o,
						  void *data);
	
	int getInt(string value);
	double getFloat(string value);
	string getString(int value);
	string getHex(int value);
	vector<char> getCharVector(string value);
	int getPreviousPowerOf2(int value);
};

typedef vector<OEComponent *> OEObserverList;

typedef struct
{
	string name;
	OEComponent *component;
} OEConnection;

typedef struct
{
	string name;
	string value;
} OEProperty;

typedef struct
{
	string name;
	vector<char> data;
} OEData;

typedef struct
{
	int id;
	OEComponent *component;
	void *data;
} OENotification;

typedef vector<string> OEMemoryRange;

typedef struct
{
	OEComponent *component;
	OEMemoryRange range;
} OEMemoryMap;

#endif
