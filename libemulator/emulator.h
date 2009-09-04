
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <string>
#include <vector>
#include <map>

using namespace std;

struct DMLTemplate
{
	string label;
	string image;
	string description;
	string group;
};

struct DMLOutlet
{
	string name;
	string type;
	string label;
	string image;
};

struct DMLInlet
{
	string name;
	string type;
	string label;
	string image;
};

struct DMLFile
{
	string label;
	string image;
	string description;
	string group;
	
	vector<DMLOutlet> outlets;
};

class Emulation
{
	Emulation();
	~Emulation();
	
	static bool readTemplates(string templatesPath, map<string, DMLTemplate> &templates);
	static bool readDMLs(string dmlsPath, map<string, DMLFile> &dmls);
	
	bool open(string emulationPath);
	bool runFrame();
	bool save(string emulationPath);
	
	bool ioctl(char * componentName, int message, void * data);
	
	bool getOutlets(vector<DMLOutlet> &outlets);
	
	bool getAvailableDMLs(map<string, DMLFile> &dmls, vector<string> &availableDMLs);
	bool getAvailableInlets(DMLOutlet &outlet, vector<DMLInlet> &availableInlets);
	
	bool addDevice(string dmlPath, map<string, string> outletInletMap);	
	void removeDevice(char * deviceName);
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
