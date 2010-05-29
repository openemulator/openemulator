
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
	
	bool setProperty(const string &name, const string &value);
	bool setResource(const string &name, const OEData &data);
	
	bool getMemoryMap(string &range);
	
	int read(int address);
	
private:
	string mappedRange;
	
	int mask;
	vector<char> memory;
};
