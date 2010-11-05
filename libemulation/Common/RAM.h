
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "OEComponent.h"

// Events
enum
{
	RAM_GET_MEMORY,
};

class RAM : public OEComponent
{
public:
	RAM();
	~RAM();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool setData(string name, OEData *data);
	bool getData(string name, OEData **data);
	
	void notify(OEComponent *sender, int notification, void *data);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEData *memory;
	OEData powerOnPattern;
	OEComponent *controlBus;
	
	OEAddress size;
	OEAddress mask;
	OEUInt8 *datap;
	
	void setSize(OEAddress size);
	void setMemory(OEData *data);
};
