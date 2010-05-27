
/**
 * libemulator
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
	
	bool setProperty(string name, string &value);
	bool setResource(string name, OEData &data);
	
	bool getMemoryMap(OEMemoryRange &range);
	
	int ioctl(int message, void *data);
	int read(int address);
	
private:
	OEMemoryRange mappedRange;
	
	int mask;
	vector<char> memory;
};
