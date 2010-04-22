
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
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	vector<string> mapVector;
	
	int size;
	int mask;
	vector<char> memory;
	vector<char> resetPattern;
};
