
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
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
void getEmulationDevices(void * emulation);
void addEmulationDevice(void * emulation, char * deviceTemplate, void * connections);
void removeEmulationDevice(void * emulation, char * deviceName);

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
void sendEmulationMessage(void * emulation, char * componentId, int message, void * data);

// Note: move pause and power off images to libemulator
