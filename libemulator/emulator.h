
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

typedef struct DMLPortNodeType
{
	char * ref;
	char * type;
	char * subtype;
	char * label;
	char * image;
	
	int isConnected;
	
	struct DMLPortNodeType *next;
} DMLPortNode;

typedef struct
{
	char * label;
	char * image;
	char * description;
	char * group;
	
	DMLPortNode * inlets;
	DMLPortNode * outlets;
} DMLInfo;

typedef struct DMLConnectionNodeType
{
	char * inletRef;
	char * outletRef;
	
	struct DMLConnectionNodeType *next;
} DMLConnectionNode;

typedef DMLConnectionNode *DMLConnections;

typedef void * Emulation;

#ifdef __cplusplus
extern "C"
{
#endif
	DMLInfo *dmlInfoRead(char * path);
	DMLInfo *dmlInfoReadFromTemplate(char * path);
	void dmlInfoFree(DMLInfo * dmlInfo);
	
	DMLConnections *dmlConnectionsNew();
	int dmlConnectionsAdd(DMLConnections * conn, char * inletRef, char * outletRef);
	void dmlConnectionsFree(DMLConnections * conn);
	
	Emulation *emulatorOpen(char * path);
	int emulatorSave(Emulation * emulation, char * path);
	void emulatorClose(Emulation * emulation);

	int emulatorIoctl(Emulation * emulation, char * componentName, int message, void * data);
	
	DMLInfo *emulatorGetDMLInfo(Emulation * emulation);
	int emulatorAddDML(Emulation * emulation, char * path, DMLConnections * conn);
	int emulatorRemoveOutlet(Emulation * emulation, char * outletRef);
#ifdef __cplusplus
}
#endif

/*
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
*/
// ioctl's for:
// * next frame
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
