
/**
 * libemulator
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "AppleISystem.h"

int AppleISystem::ioctl(int message, void *data)
{
	// When connected to host::audio, receive notifications from audio buffers
	return 0;	
}
