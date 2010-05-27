
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
	
	bool setProperty(string name, string value);
	bool connect(string name, OEComponent *component);
	
	bool getMemoryMap(string &range);
	
	int read(int address);
	void write(int address, int value);
	
private:
	string mappedRange;
	
	int offset;
	
	OEComponent *connectedComponent;
};
