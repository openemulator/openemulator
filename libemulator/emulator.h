
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
	
	static bool readDML(char *dmlInfo, int dmlInfoSize, DMLInfo &dmlInfo);
	static bool readDML(string path, DMLInfo &dmlInfo);
	static bool readTemplate(string path, DMLInfo &dmlInfo);
	
	bool open(string path);
	bool runFrame();
	bool save(string path);
	
	bool ioctl(string componentName, int message, void * data);
	
	bool getOutlets(vector<DMLOutlet> &outlets);
	bool getInlets(vector<DMLInlet> &inlets);
	
	bool getAvailableDMLs(map<string, DMLInfo> &dmls, vector<string> &availableDMLs);
	bool getAvailableInlets(DMLOutlet &outlet, vector<DMLInlet> &availableInlets);
	
	bool addDML(string path, map<string, string> outletInletMap);	
	void removeDevicesFromOutlet(string outletName);
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
