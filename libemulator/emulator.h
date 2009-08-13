/*
 *  emulator.h
 *  OpenEmulator
 *
 *  Created by Marc S. Reßl on 11/08/09.
 *  Copyright 2009 ITBA. All rights reserved.
 *
 */

// Returns: {templatePath, platform, group, label, description, image,
//			 {componentName, outlet {propertyKey, propertyValue, inlet}}
//			}
void getEmulationTemplates(char * paths);

void * openEmulationFromTemplate(char * path);
void * openEmulation(char * path);
void runEmulationFrame(void * emulation);
void saveEmulationAs(void * emulation, char * path, int saveAsPackage);
void closeEmulation(void * emulation);

// This returns: deviceName, deviceLabel, deviceDescription, deviceImage, {componentId + outlet {property + inlet}}
void getEmulationDeviceList(void * emulation);
void addEmulationDevice(void * emulation, char * deviceTemplate, void * connections);
void removeEmulationDevice(void * emulation, char * deviceId);

// We use ioctl's to send power, reset, pause messages
// We use ioctl's to do copy, paste, isCopyAvailable
// We use ioctl's to get/set video options
// We use ioctl's to get/set audio volume
// We use ioctl's to play/record audio
// We use ioctl's to lock/unlock disk drives
// We use ioctl's to mount/unmount disk drives
// We use ioctl's to get/set component options
// We use ioctl's to send keyboard messages
// We use ioctl's to set mouse position and buttons
// We use ioctl's to set joystick position and buttons
// We use ioctl's to set graphics tablet position and buttons
// We use ioctl's to get video frames
void sendEmulationIOCTL(void * emulation, char * componentId, int message, void * data);

// Note: move pause and power off images to libemulator
