
/**
 * libemulation
 * Generic RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic RAM segment
 */

#include "OEComponent.h"

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
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	OEComponent *host;
	string mmuMap;
	
	bool isPowered;
	
	OEData *memory;
	OEData resetPattern;
	
	int size;
	int mask;
	char *datap;
	
	void setSize(int size);
	void setMemory(OEData *data);
};
