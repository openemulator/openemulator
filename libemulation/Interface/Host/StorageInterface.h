
/**
 * libemulation
 * Storage Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the storage interface.
 */

#ifndef _STORAGEINTERFACE_H
#define _STORAGEINTERFACE_H

// Notes:
// * isAvailable() tests if the device is available for mounting images.
//   This is used when an emulation searches for a free device to mount an image.
// * test() tests if an image is can be mounted in this device.
// * mount() force-mounts. It should attempt to mount the image by all means.
//   If an error occurs, a previously mounted image should remain mounted.
// * unmount() force-unmounts. It should attempt to unmount the image by all means.
// * getMountPath() returns the path of the mounted image.
// * getStateLabel() returns a state label describing the mount.
//   E.g.: "Apple II 16 sector, 35 track, volume 254, read/write".
// * isLocked() indicates if the disk is physically locked in the emulated device.

typedef enum
{
	STORAGE_IS_AVAILABLE,
	STORAGE_TEST,
	STORAGE_MOUNT,
	STORAGE_UNMOUNT,
	STORAGE_GET_MOUNTPATH,
	STORAGE_GET_STATELABEL,
	STORAGE_IS_LOCKED,
} StorageMessage;

#endif
