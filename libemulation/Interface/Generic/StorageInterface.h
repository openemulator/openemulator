
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
	STORAGE_MOUNT,
	STORAGE_UNMOUNT,
	STORAGE_IS_MOUNTABLE,
	STORAGE_IS_MOUNTED,
	STORAGE_IS_WRITABLE,
	STORAGE_IS_LOCKED,
	STORAGE_GET_PATH,
	STORAGE_GET_FORMAT,
	STORAGE_GET_CAPACITY,
} StorageMessages;
