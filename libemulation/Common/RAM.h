
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
	
	bool setProperty(const string &name, const string &value);
	bool setData(const string &name, OEData *data);
	bool getData(const string &name, OEData **data);
	bool connect(const string &name, OEComponent *component);
	
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
