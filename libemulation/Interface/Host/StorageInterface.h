
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
// * isAvailable() determines if the device is (now) available for mounting images.
//   This is used when an emulation searches for an available device to mount an image.
// * canMount() tests if an image can be mounted (some time) in this device.
// * mount() force-mounts. It should attempt to mount the image by all means.
//   If an error occurs, a previously mounted image should remain mounted.
// * unmount() force-unmounts. It should attempt to unmount the image by all means.
// * getMountPath() returns the path of the mounted image.
// * getStateLabel() returns a state label describing the mount.
//   E.g.: "16 sectors, 35 track, read-only".
// * isLocked() indicates if the disk is physically locked in the emulated device.

typedef enum
{
	STORAGE_IS_AVAILABLE,
	STORAGE_CAN_MOUNT,
	STORAGE_MOUNT,
	STORAGE_UNMOUNT,
	STORAGE_GET_MOUNTPATH,
	STORAGE_IS_LOCKED,
	
	STORAGE_GET_FORMATLABEL,
	
	STORAGE_SET_SETTINGS,
	STORAGE_GET_SETTINGS,
} StorageMessage;

typedef struct
{
	string ref;
	string name;
	string label;
	string type;
	string options;
} StorageSetting;

typedef vector<StorageSetting> StorageSettings;

#endif
