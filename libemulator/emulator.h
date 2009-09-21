
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

typedef void * DMLEmulation;

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
	
	DMLEmulation *emulatorOpen(char * emulationPath, char * resourcePath);
	int emulatorSave(DMLEmulation * dmlEmulation,
					 char * path);
	void emulatorClose(DMLEmulation * dmlEmulation);
	
	int emulatorIoctl(DMLEmulation * dmlEmulation,
					  char * componentName, int message, void * data);
	
	DMLInfo *emulatorGetDMLInfo(DMLEmulation * dmlEmulation);
	int emulatorAddDML(DMLEmulation * dmlEmulation,
					   char * path, DMLConnections * conn);
	int emulatorRemoveOutlet(DMLEmulation * dmlEmulation,
							 char * outletRef);
#ifdef __cplusplus
}
#endif
