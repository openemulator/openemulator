
/**
 * libemulator
 * Host System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host system events
 */

#include "OEComponent.h"

// Messages
enum
{
	HOST_SYSTEM_UPDATE_DEVICELIST = COMPONENT_USER,
	HOST_SYSTEM_IS_DEVICELIST_UPDATED,
};
