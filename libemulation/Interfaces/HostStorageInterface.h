
/**
 * libemulation
 * Host storage interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host storage interface
 */

#ifndef _HOSTSTORAGEINTERFACE_H
#define _HOSTSTORAGEINTERFACE_H

//
// * The framework first tries to mount on all available devices.
// * If the mount failed, it tries to check whether a drive is capable of processing
//   the image (validation), for displaying an appropriate error message
// * A loader is a component that just loads the information
//   (e.g. binary BIN code fragments, PRG, T64 formats)
// * A mounter is a component that is capable of holding the information while mounted
//   (e.g. disk drives, cartridges)
// * It is up to the mounter to automatically eject a disk image
// * To unmount, the framework sends an empty string
// * To update mount status, use the device status interface
//

typedef enum
{
	HOST_STORAGE_RUNALERT,
} HostStorageMessages;

typedef enum
{
	HOST_STORAGE_MOUNT,
	HOST_STORAGE_VALIDATE,
} HostStorageDelegations;

#endif
