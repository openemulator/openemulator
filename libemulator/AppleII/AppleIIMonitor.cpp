
/**
 * libemulator
 * Apple II Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II monitor.
 */

#include <math.h>

#include "AppleIIMonitor.h"

int AppleIIMonitor::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
			break;
	}
	
	return false;
}
