
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
	HOST_EVENTS_UPDATE_DEVICELIST = COMPONENT_USER,
	HOST_EVENTS_IS_DEVICELIST_UPDATED,
	HOST_EVENTS_POWER_UP,
	HOST_EVENTS_POWER_DOWN,
	HOST_EVENTS_RESET_UP,
	HOST_EVENTS_RESET_DOWN,
	HOST_EVENTS_NMI_UP,
	HOST_EVENTS_NMI_DOWN,
};
