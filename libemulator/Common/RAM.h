
/**
 * libemulator
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
	
	bool setProperty(string name, string &value);
	bool setData(string name, OEData &data);
	bool getData(string name, OEData &data);
	bool connect(string name, OEComponent *component);
	void notify(int notification, OEComponent *component);
	
	int read(int address);
	void write(int address, int value);
	
private:
	OEMemoryRange mappedRange;
	
	int size;
	int mask;
	vector<char> memory;
	vector<char> resetPattern;
};
