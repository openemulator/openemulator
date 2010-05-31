
/**
 * libemulator
 * Generic memory offset logic
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory offset logic
 */

#include "OEComponent.h"

class MemoryOffset : public OEComponent
{
public:
	MemoryOffset();
	
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);
	
	bool getMemoryMap(string &range);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	string mappedRange;
	
	int offset;
	
	OEComponent *connectedComponent;
};
