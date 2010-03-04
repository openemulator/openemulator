
/**
 * libemulator
 * Random generator
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates random output
 */

#include "OEComponent.h"

class Random : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	
private:
	int offset;
	int size;
};
