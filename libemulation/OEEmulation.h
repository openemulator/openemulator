
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

typedef void (*OERunAlertCallback)(string message);
typedef OEComponent *(*OEAddCanvasCallback)(void *userData);
typedef void (*OERemoveCanvasCallback)(OEComponent *canvas, void *userData);

typedef enum
{
	EMULATION_SET_DEVICEINFO,
	EMULATION_ADD_CANVAS,
	EMULATION_REMOVE_CANVAS,
	EMULATION_RUN_ALERT,
} OEEmulationMessages;

typedef enum
{
	EMULATION_MOUNT,
	EMULATION_VALIDATE,
} OEEmulationDelegations;

typedef struct
{
	string ref;
	string property;
	string type;
	string options;
	string label;
} OESettingInfo;

typedef vector<OESettingInfo> OESettingsInfo;

typedef struct
{
	string id;
	string label;
	string image;
	
	string connectionLabel;
	string informativeText;
	OEComponent *canvas;
	OEComponent *storage;
	
	OESettingsInfo settingsInfo;
} OEDeviceInfo;

typedef vector<OEDeviceInfo> OEDevicesInfo;

typedef struct
{
	string informativeText;
	OEComponent *canvas;
	OEComponent *storage;
} OEEmulationDeviceInfo;

typedef map<string, OEEmulationDeviceInfo> OEEmulationDevicesInfo;

typedef struct
{
	string deviceId;
	OEEmulationDeviceInfo deviceInfo;
} OEEmulationSetDeviceInfo;

typedef map<string, OEComponent *> OEComponentsMap;
typedef map<string, string> OEPropertiesMap;

class OEEmulation : public OEComponent, public OEEDL
{
public:
	OEEmulation();
	~OEEmulation();
	
	void setResourcePath(string path);
	void setAlertCallback(OERunAlertCallback alertCallback);
	void setAddCanvasCallback(OEAddCanvasCallback addCanvasCallback,
							  void *userData);
	void setRemoveCanvasCallback(OERemoveCanvasCallback removeCanvasCallback,
								 void *userData);
	
	bool open(string path);
	bool save(string path);
	void close();
	
	OEDevicesInfo getDevicesInfo();
	bool setComponent(string id, OEComponent *component);
	bool addEDL(string path, OEIdMap idMap);
	bool removeDevice(string id);
	bool mount(string path);
	bool validate(string path);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	OEComponentsMap components;
	
	OEEmulationDevicesInfo devicesInfo;
	
	OERunAlertCallback alertCallback;
	OEAddCanvasCallback addCanvasCallback;
	OERemoveCanvasCallback removeCanvasCallback;
	void *addCanvasCallbackUserData;
	void *removeCanvasCallbackUserData;
	
	OEComponent *getComponent(string id);
	
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
	
	bool hasXMLProperty(string value, string propertyName);
	string parseXMLProperties(string value, OEPropertiesMap &propertiesMap);
};

#endif
