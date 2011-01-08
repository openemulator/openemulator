
/**
 * libemulation
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#ifndef _EMULATION_H
#define _EMULATION_H

#include "OEEDL.h"
#include "OEComponent.h"

// Notes:
// * The framework first tries to mount on all available devices.
// * If the mount failed, it tries to check whether a drive is capable of processing
//   the image (validation), for displaying an appropriate error message
// * A loader is a component that just loads the information
//   (e.g. binary BIN code fragments, PRG, T64 formats)
// * A mounter is a component that is capable of holding the information while mounted
//   (e.g. disk drives, cartridges)
// * It is up to the mounter to automatically eject a disk image
// * To unmount, an empty string is used
// * To update mount status, use the device info interface

typedef enum
{
	EMULATION_SET_STATE,
	EMULATION_SET_IMAGE,
	EMULATION_SET_POWERED,
	EMULATION_CREATE_CANVAS,
	EMULATION_DESTROY_CANVAS,
	EMULATION_ADD_STORAGE,
	EMULATION_REMOVE_STORAGE,
	
	EMULATION_RUN_ALERT,
	EMULATION_UPDATE,
} EmulationMessages;



typedef struct
{
	string ref;
	string name;
	string label;
	string type;
	string options;
} EmulationSetting;

typedef vector<EmulationSetting> EmulationSettings;

typedef struct
{
	string id;
	string label;
	string image;
	string group;
	EmulationSettings settings;
	
	string location;
	
	string state;
	bool powered;
	OEComponents canvases;
	OEComponents storages;
} EmulationDeviceInfo;

typedef vector<EmulationDeviceInfo> EmulationInfo;



typedef void (*EmulationRunAlert)(void *userData, string message);
typedef OEComponent *(*EmulationAddCanvas)(void *userData);
typedef void (*EmulationRemoveCanvas)(void *userData, OEComponent *canvas);
typedef void (*EmulationDidUpdate)(void *userData);



class Emulation : public OEComponent, public OEEDL
{
public:
	Emulation();
	~Emulation();
	
	void setResourcePath(string path);
	void setRunAlert(EmulationRunAlert runAlert);
	void setAddCanvas(EmulationAddCanvas addCanvas);
	void setRemoveCanvas(EmulationRemoveCanvas removeCanvas);
	void setDidUpdate(EmulationDidUpdate didUpdate);
	void setUserData(void *userData);
	
	bool open(string path);
	bool save(string path);
	
	OEComponent *getComponent(string id);
	bool setComponent(string id, OEComponent *component);
	
	EmulationInfo *getEmulationInfo();
	bool addEDL(string path, map<string, string> idMap);
	bool removeDevice(string id);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	map<string, OEComponent *> componentsMap;
	EmulationInfo emulationInfo;
	
	EmulationRunAlert runAlert;
	EmulationAddCanvas addCanvas;
	EmulationRemoveCanvas removeCanvas;
	EmulationDidUpdate didUpdate;
	void *userData;
	
	void buildEmulationInfo();
	EmulationSettings buildDeviceSettings(xmlNodePtr children);
	string buildDeviceLocation(string deviceId);
	string buildDeviceLocation(string deviceId, vector<string> &visitedDevices);
	EmulationDeviceInfo *getDeviceInfo(string id);
	
	bool dumpEmulation(OEData *data);
	bool createEmulation();
	bool createComponent(string id, string className);
	bool configureEmulation();
	bool configureComponent(string id, xmlNodePtr children);
	bool initEmulation();
	bool initComponent(string id);
	bool updateEmulation();
	bool updateComponent(string id, xmlNodePtr children);
	void disconnectEmulation();
	void disconnectComponent(string id, xmlNodePtr children);
	void destroyEmulation();
	void destroyComponent(string id, xmlNodePtr children);
	
	bool hasValueProperty(string value, string propertyName);
	string parseValueProperties(string value, map<string, string> &propertiesMap);
	
	void setDeviceId(string &id, string deviceId);
	string getDeviceId(string id);
	string getDeviceId(OEComponent *component);
};

#endif
