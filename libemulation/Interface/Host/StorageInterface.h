
/**
 * libemulation
 * Storage Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the storage interface.
 */

// Notes:
// * isAvailable() determines if the device is available (now) for mounting images.
//   This is used when an emulation searches for an available device to mount an image.
// * canMount() tests if an image can be mounted (at some time) in this device.
// * mount() force-mounts. It should attempt to mount the image by all means.
//   If an error occurs, a previously mounted image should remain mounted.
// * unmount() force-unmounts. It should attempt to unmount the image by all means.
// * getMountPath() returns the path of the mounted image.
// * isLocked() indicates if the disk is physically locked in the emulated device.
// * getFormatLabel() returns a format label describing the mount.
//   E.g.: "16 sectors, 35 track, read-only".

#ifndef _STORAGEINTERFACE_H
#define _STORAGEINTERFACE_H

typedef enum
{
    STORAGE_IS_AVAILABLE,
    STORAGE_CAN_MOUNT,
    STORAGE_MOUNT,
    STORAGE_UNMOUNT,
    STORAGE_GET_MOUNTPATH,
    STORAGE_IS_LOCKED,
    
    STORAGE_GET_FORMATLABEL,
    
    STORAGE_GET_SETTINGS,
} StorageMessage;

#endif
