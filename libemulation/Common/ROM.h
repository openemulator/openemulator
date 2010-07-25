
/**
 * libemulation
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "OEComponent.h"

class ROM : public OEComponent
{
public:
	ROM();
	~ROM();
	
	bool setProperty(const string &name, const string &value);
	bool setResource(const string &name, OEData *data);
	
	bool getMemoryMap(string &range);
	
	OEUInt8 read(int address);
	
private:
	string mappedRange;
	
	OEData *memory;
	int mask;
	char *data;
	
	void updateMemory(int size);
};
