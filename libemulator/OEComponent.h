
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
	OEIOCTL_CONNECT,
	OEIOCTL_SET_PROPERTY,
	OEIOCTL_GET_PROPERTY,
	OEIOCTL_SET_DATA,
	OEIOCTL_GET_DATA,
	OEIOCTL_SET_RESOURCE,
	OEIOCTL_GET_MEMORYMAP,
	OEIOCTL_SET_MEMORYMAP,
	OEIOCTL_NOTIFY,
	OEIOCTL_ASSERT_IRQ,
	OEIOCTL_RELEASE_IRQ,
	OEIOCTL_USER,
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
	
	void addObserver(class OEComponent *component);
	void removeObserver(class OEComponent *component);
	
protected:
	void postNotification(int message, void *data);
	
private:
	vector<OEComponent *> observers;
};

int intValue(string value);
int nextPowerOf2(int value);

typedef struct
{
	string name;
	OEComponent *component;
} OEIoctlConnection;

typedef struct
{
	string name;
	string value;
} OEIoctlProperty;

typedef struct
{
	string name;
	vector<char> data;
} OEIoctlData;

typedef struct
{
	OEComponent *component;
	int offset;
	int size;
} OEIoctlMemoryMap;

typedef struct
{
	OEComponent *component;
	int message;
	void *data;
} OEIoctlNotification;

#endif
