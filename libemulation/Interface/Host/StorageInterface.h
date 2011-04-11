
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
// * isMountPermitted() checks if an image can be mounted now in this device.
//   This is used when an emulation searches for an available device for mounting an image.
// * isMountPossible() checks if an image can be mounted sometime in this device.
//   This is used for determining the correct error message when an emulation attempted
//   to mount an image somewhere but could not.
// * mount() force-mounts. If there was an error and there was a previous image, the
//   previous image should not be unmounted. Otherwise it should attempt to mount the image
//   by all means.
// * unmount() force-unmounts. It should attempt to unmount the image by all means.
// * getMountPath() returns the path of the mounted image.
// * getStateLabel() returns a state label describing the mount.
//   For example: "Apple II 16 sector, 35 track, read-write".
// * isLocked() indicates that a device is locking the image. This is used for displaying
//   a warning message when unmounting.

typedef enum
{
	STORAGE_IS_MOUNT_PERMITTED,
	STORAGE_IS_MOUNT_POSSIBLE,
	STORAGE_MOUNT,
	STORAGE_UNMOUNT,
	STORAGE_GET_MOUNTPATH,
	STORAGE_GET_STATELABEL,
	STORAGE_IS_LOCKED,
} StorageMessage;

#endif
