
/**
 * libemulation
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#ifndef _OEEMULATION_H
#define _OEEMULATION_H

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
	OEEMULATION_SET_STATE,
	OEEMULATION_SET_IMAGE,
	OEEMULATION_SET_POWERED,
	OEEMULATION_ADD_CANVAS,
	OEEMULATION_REMOVE_CANVAS,
	OEEMULATION_ADD_STORAGE,
	OEEMULATION_REMOVE_STORAGE,
	
	OEEMULATION_RUN_ALERT,
	OEEMULATION_UPDATE_DEVICES,
	
	OEEMULATION_MOUNT,
	OEEMULATION_IS_MOUNTABLE,
	OEEMULATION_GET_MOUNT_PATH,
	OEEMULATION_IS_LOCKED,
} OEEmulationMessages;



typedef struct
{
	string ref;
	string name;
	string label;
	string type;
	string options;
} OESetting;
typedef vector<OESetting> OESettings;

typedef struct
{
	string id;
	string group;
	string label;
	string image;
	OESettings settings;
	string location;
	bool removable;
	
	string state;
	bool powered;
	OEComponents canvases;
	OEComponents storages;
} OEDeviceInfo;
typedef vector<OEDeviceInfo> OEDevicesInfo;



typedef map<string, OEComponent *> OEComponentsMap;

typedef map<string, string> OEPropertiesMap;



typedef void (*OERunAlertCallback)(string message);
typedef OEComponent *(*OEAddCanvasCallback)(void *userData);
typedef void (*OERemoveCanvasCallback)(OEComponent *canvas, void *userData);
typedef void (*OEDevicesDidUpdateCallback)();



class OEEmulation : public OEComponent, public OEEDL
{
public:
	OEEmulation();
	~OEEmulation();
	
	void setResourcePath(string path);
	void setRunAlertCallback(OERunAlertCallback runAlert);
	void setAddCanvasCallback(OEAddCanvasCallback addCanvas,
							  void *userData);
	void setRemoveCanvasCallback(OERemoveCanvasCallback removeCanvas,
								 void *userData);
	void setDevicesDidUpdateCallback(OEDevicesDidUpdateCallback
									 devicesDidUpdate);
	
	bool open(string path);
	bool save(string path);
	
	OEComponent *getComponent(string id);
	bool setComponent(string id, OEComponent *component);
	
	OEDevicesInfo &getDevicesInfo();
	bool addEDL(string path, OEIdMap idMap);
	bool removeDevice(string id);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	OEComponentsMap componentsMap;
	OEDevicesInfo devicesInfo;
	
	OERunAlertCallback runAlert;
	OEAddCanvasCallback addCanvas;
	void *addCanvasUserData;
	OERemoveCanvasCallback removeCanvas;
	void *removeCanvasUserData;
	OEDevicesDidUpdateCallback devicesDidUpdate;
	
	void buildDevicesInfo();
	OESettings buildDeviceSettings(xmlNodePtr children);
	string buildDeviceLocation(string deviceId);
	string buildDeviceLocation(string deviceId, vector<string> &visitedDevices);
	OEDeviceInfo *getDeviceInfo(string id);
	
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
	string parseValueProperties(string value, OEPropertiesMap &propertiesMap);
	
	void setDeviceId(string &id, string deviceId);
	string getDeviceId(string id);
	string getDeviceId(OEComponent *component);
};

#endif
