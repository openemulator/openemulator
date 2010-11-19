
/**
 * libemulation
 * Host storage interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host storage interface
 */

#ifndef _HOSTEMULATIONCONTROLLERINTERFACE_H
#define _HOSTEMULATIONCONTROLLERINTERFACE_H

//
// Storage:
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
	HOST_EMULATIONCONTROLLER_SET_INFO,
	HOST_EMULATIONCONTROLLER_GET_INFOS,
	HOST_EMULATIONCONTROLLER_ADD_CANVAS,
	HOST_EMULATIONCONTROLLER_REMOVE_CANVAS,
	HOST_EMULATIONCONTROLLER_RUN_ALERT,
} HostEmulationControllerMessages;

typedef enum
{
	HOST_EMULATIONCONTROLLER_MOUNT,
	HOST_EMULATIONCONTROLLER_VALIDATE,
} HostEmulationControllerDelegations;

typedef struct
{
	string deviceId;
	string informativeText;
	OEComponent *canvas;
	HostEmulationController isStorageMounted;
} HostEmulationControllerInfo;

typedef vector<HostEmulationControllerInfo> HostEmulationControllerInfos;

#endif
