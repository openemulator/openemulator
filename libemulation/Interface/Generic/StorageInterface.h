
/**
 * libemulation
 * Storage Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the storage interface
 */

#include "OEComponent.h"

typedef enum
{
	STORAGE_MOUNT,
	STORAGE_IS_MOUNTABLE,
	STORAGE_GET_MOUNT_PATH,
	STORAGE_IS_LOCKED,
} StorageMessages;
