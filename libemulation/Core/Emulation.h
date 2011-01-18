
/**
 * libemulation
 * Emulation
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
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
	EMULATION_UPDATE,
	EMULATION_RUN_ALERT,
	EMULATION_ASSERT_ACTIVE,
	EMULATION_CLEAR_ACTIVE,
	
	EMULATION_SET_STATE,
	EMULATION_SET_IMAGE,
	EMULATION_SET_HOT_PLUGGABLE,
	EMULATION_CREATE_CANVAS,
	EMULATION_DESTROY_CANVAS,
	EMULATION_SET_STORAGE,
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
	bool hotPluggable;
	OEComponents canvases;
	OEComponent *storage;
} EmulationDeviceInfo;

typedef vector<EmulationDeviceInfo> EmulationInfo;



typedef void (*EmulationDidUpdate)(void *userData);
typedef void (*EmulationRunAlert)(void *userData, string message);
typedef OEComponent *(*EmulationCreateCanvas)(void *userData, string title);
typedef void (*EmulationDestroyCanvas)(void *userData, OEComponent *canvas);



class Emulation : public OEComponent, public OEEDL
{
public:
	Emulation();
	~Emulation();
	
	void setResourcePath(string path);
	void setDidUpdate(EmulationDidUpdate didUpdate);
	void setRunAlert(EmulationRunAlert runAlert);
	void setCreateCanvas(EmulationCreateCanvas createCanvas);
	void setDestroyCanvas(EmulationDestroyCanvas destroyCanvas);
	void setUserData(void *userData);
	
	bool open(string path);
	bool save(string path);
	
	OEComponent *getComponent(string id);
	bool setComponent(string id, OEComponent *component);
	
	EmulationInfo *getEmulationInfo();
	bool addEDL(string path, map<string, string> idMap);
	bool removeDevice(string id);
	
	bool isActive();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	map<string, OEComponent *> componentsMap;
	EmulationInfo emulationInfo;
	
	EmulationDidUpdate didUpdate;
	EmulationRunAlert runAlert;
	EmulationCreateCanvas createCanvas;
	EmulationDestroyCanvas destroyCanvas;
	void *userData;
	
	int activeCount;
	
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
