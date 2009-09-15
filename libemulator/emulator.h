
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

typedef struct DMLPortType
{
	char * name;
	char * type;
	char * label;
	char * image;
	
	int connected;
	
	struct DMLPortType *next;
} DMLPort;

typedef struct
{
	char * label;
	char * image;
	char * description;
	char * group;
	
	DMLPort * outlets;
} DMLInfo;

typedef struct
{
	
	
} Emulation;

DMLInfo *dmlInfoRead(char * path);
DMLInfo *dmlInfoReadFromTemplate(char * path);
void dmlInfoFree(DMLInfo * dmlInfo);

Emulation *emulatorOpen(char *path);
int emulatorRunFrame(Emulation *emulation);
int emulatorSave(Emulation *emulation, char *path);
int emulatorClose(Emulation *emulation);

int emulatorIoctl(char *componentName, int message, void * data);

DMLPort *emulatorGetOutlets(Emulation *emulation);
DMLPort *emulatorGetInlets(Emulation *emulation);

int emulatorAddDML(char *path);
void emulatorRemoveOutlet(char *outletName);



class Emulation
{
public:
	Emulation();
	~Emulation();
	
	static bool readDML(char *dmlData, int dmlDataSize, DMLInfo &dmlInfo);
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
	void removeDevicesOnOutlet(string outletName);
};

// ioctl's for:
// * update outlets
// * send power, reset, pause messages
// * send config messages
// * do copy, paste, isCopyAvailable
// * get/set video options
// * get/set audio volume
// * play/record audio
// * lock/unlock disk drives
// * mount/unmount disk drives
// * get/set component options
// * send keyboard messages
// * set mouse position and buttons
// * set joystick position and buttons
// * set graphics tablet position and buttons
// * get video frames

// Note: move pause and power off images to libemulator
