
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

struct DMLInfo
{
	string label;
	string image;
	string description;
	string group;
	
	vector<DMLOutlet> outlets;
};

class Emulation
{
public:
	Emulation();
	~Emulation();
	
	static bool readTemplates(string templatesPath, map<string, DMLInfo> &templates);
	static bool readDMLs(string dmlsPath, map<string, DMLInfo> &dmls);
	
	bool open(string emulationPath);
	bool runFrame();
	bool save(string emulationPath);
	
	bool ioctl(string componentName, int message, void * data);
	
	bool getOutlets(vector<DMLOutlet> &outlets);
	
	bool getAvailableDMLs(map<string, DMLInfo> &dmls, vector<string> &availableDMLs);
	bool getAvailableInlets(DMLOutlet &outlet, vector<DMLInlet> &availableInlets);
	
	bool addDevice(string dmlPath, map<string, string> outletInletMap);	
	void removeDevice(string deviceName);
	
private:
	static bool readDML(string path, DMLInfo &dmlInfo);
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
