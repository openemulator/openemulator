
/**
 * libemulator
 * Apple II Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II monitor.
 */

#include "OEComponent.h"

class AppleIIMonitor : public OEComponent
{
public:
	int ioctl(int message, void *data);

};
