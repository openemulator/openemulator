
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "OEComponent.h"

// Messages
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
	bool init();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
	void notify(OEComponent *sender, int notification, void *data);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEData powerOnPattern;
	OEComponent *controlBus;
	
	OEAddress size;
	OEAddress mask;
	OEData *data;
	OEUInt8 *datap;
};
