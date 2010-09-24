
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
	bool setComponent(string name, OEComponent *component);
	bool setData(string name, OEData *data);
	bool getData(string name, OEData **data);
	
	void notify(OEComponent *component, int notification, void *data);
	
	bool postEvent(OEComponent *component, int event, void *data);
	
	int read(int address);
	void write(int address, int value);
	
private:
	OEData *memory;
	OEData powerOnPattern;
	
	OEComponent *host;
	
	int size;
	int mask;
	char *datap;
	
	void setSize(int size);
	void setMemory(OEData *data);
};
