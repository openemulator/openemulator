
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <vector>
#include <map>

using namespace std;

class Emulation
{
	Emulation();
	~Emulation();
	
	static vector<string> getTemplates(string type, string platform); // Returns a list of devicePath's
	static string getTemplateGroup(string templatePath);
	static string getTemplateLabel(string templatePath);
	static string getTemplateImage(string templatePath);
	vector<string> string getTemplateOutlets(string templatePath);
	
	bool open(char * emulationPath);
	bool runFrame();
	bool save(char * emulationPath);	// Automatically handles packaging, according to extension
	
	vector<string> getOutlets(string type); // Returns a list of deviceName:componentName
	string getOutletLabel(string componentName);
	string getOutletImage(string componentName);	// Gets image property. if not available, uses device image
	
	vector<string> getFreeInlets(string type); // Returns a list of deviceName:componentName:property
	string getInletLabel(string propertyName);
	
	bool addDevice(char * templatePath, map<string, string> connections); // inlet > outlet
	void removeDevice(char * deviceName);
	
	void ioctl(char * componentName, int message, void * data);
};

// We use ioctl's to update outlets
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
