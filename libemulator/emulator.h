
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
	string path;
	string label;
	string image;
	string description;
	string group;
};

struct DMLOutlet
{
	string componentName;
	string outletType;
	string label;
	string image;
};

struct DMLInlet
{
	string propertyName;
	string inletType;
	string label;
	string image;
};

struct DMLFile
{
	string path;
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
	
	static int readTemplates(char * templatesPath, vector<DMLTemplate> &templates);
	static int readDMLs(char * dmlsPath, vector<DMLFile> &dmls);
	
	bool open(char * emulationPath);
	bool runFrame();
	bool save(char * emulationPath);	// Makes zipped packages, if extension does not end in /
	
	void ioctl(char * componentName, int message, void * data);
	
	int getOutlets(vector<DMLOutlet> &outlets);
	
	int getAvailableDMLs(vector<DMLFile> &dmls, vector<DMLFile> &availableDMLs);
	int getAvailableInlets(DMLOutlet &outlet, vector<DMLInlet> &availableInlets);
	
	bool addDevice(char * dmlPath, map<string, string> outletInletMap);	
	void removeDevice(char * deviceName);
};

// For adding an emulation
// 1) Search all .emulation files and packages
// 2) getDMLInfo for every emulation
// 3) Return a vector with all this information

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
