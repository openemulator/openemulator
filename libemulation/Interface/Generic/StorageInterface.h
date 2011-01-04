
/**
 * libemulation
 * Storage Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the storage interface.
 */

typedef enum
{
	STORAGE_IS_IMAGE_MOUNTABLE,
	STORAGE_MOUNT_IMAGE,
	STORAGE_UNMOUNT_IMAGE,
	STORAGE_IS_LOCKED,
	STORAGE_GET_IMAGE_PATH,
	STORAGE_GET_FORMAT,
	STORAGE_GET_CAPACITY,
} StorageMessages;
