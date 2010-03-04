
/**
 * libemulator
 * Zero generator
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates zero output
 */

#include "OEComponent.h"

class Zero : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	
private:
	int offset;
	int size;
};
