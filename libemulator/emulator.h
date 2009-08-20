
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

class Emulation
{
	Emulation();
	~Emulation();
	
	static map getTemplates(char * paths);
	
	void runFrame();
	void open(char * path);
	void save(void * emulation, char * path, int saveAsPackage);
	
	void getDeviceList();
	void addDevice(char * templatePath, void * connections);
	void removeDevice(char * deviceName);
	
	void ioctl(char * componentId, int message, void * data);
}

// We use ioctl's to send power, reset, pause messages
// We use ioctl's to send config messages
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

// Note: move pause and power off images to libemulator
