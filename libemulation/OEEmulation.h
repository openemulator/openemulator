
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

// Storage
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
	EMULATION_SET_DEVICESTATE,
	EMULATION_ADD_CANVAS,
	EMULATION_REMOVE_CANVAS,
	EMULATION_ADD_STORAGE,
	EMULATION_REMOVE_STORAGE,
	EMULATION_RUN_ALERT,
} OEEmulationMessages;

typedef enum
{
	EMULATION_MOUNT,
	EMULATION_MOUNTABLE,
} OEEmulationDelegations;

typedef struct
{
	string ref;
	string name;
	string type;
	string options;
	string label;
} OESetting;
typedef vector<OESetting> OESettings;

typedef struct
{
	string label;
	string image;
	OESettings settings;
	
	string location;
	
	string state;
	OEComponents canvases;
	OEComponents storages;
} OEDeviceInfo;
typedef map<string, OEDeviceInfo> OEDevicesInfoMap;

typedef map<string, OEComponent *> OEComponentsMap;

typedef map<string, string> OEPropertiesMap;

typedef void (*OEDevicesInfoMapDidUpdateCallback)();
typedef void (*OERunAlertCallback)(string message);
typedef OEComponent *(*OEAddCanvasCallback)(void *userData);
typedef void (*OERemoveCanvasCallback)(OEComponent *canvas, void *userData);

class OEEmulation : public OEComponent, public OEEDL
{
public:
	OEEmulation();
	~OEEmulation();
	
	void setResourcePath(string path);
	void setDevicesInfoMapDidUpdateCallback(OEDevicesInfoMapDidUpdateCallback
											devicesInfoDidUpdate);
	void setAlertCallback(OERunAlertCallback runAlert);
	void setAddCanvasCallback(OEAddCanvasCallback addCanvas,
							  void *userData);
	void setRemoveCanvasCallback(OERemoveCanvasCallback removeCanvas,
								 void *userData);
	
	bool open(string path);
	bool save(string path);
	
	bool setComponent(string id, OEComponent *component);
	
	OEDevicesInfoMap getDevicesInfoMap();
	bool addEDL(string path, OEIdMap idMap);
	bool removeDevice(string id);
	
	bool mount(string path);
	bool mountable(string path);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	OEComponentsMap componentsMap;
	OEDevicesInfoMap devicesInfoMap;
	
	OEDevicesInfoMapDidUpdateCallback devicesInfoMapDidUpdate;
	OERunAlertCallback runAlert;
	OEAddCanvasCallback addCanvas;
	void *addCanvasUserData;
	OERemoveCanvasCallback removeCanvas;
	void *removeCanvasUserData;
	
	OEComponent *getComponent(string id);
	string getDeviceId(OEComponent *component);
	
	bool dumpEmulation(OEData *data);
	void parseEmulation();
	OESettings parseDevice(xmlNodePtr children);
	string parseLocation(string deviceId, vector<string> &visitedDevices);
	
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
};

#endif
