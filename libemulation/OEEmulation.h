
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

//
// Storage:
// * The framework first tries to mount on all available devices.
// * If the mount failed, it tries to check whether a drive is capable of processing
//   the image (validation), for displaying an appropriate error message
// * A loader is a component that just loads the information
//   (e.g. binary BIN code fragments, PRG, T64 formats)
// * A mounter is a component that is capable of holding the information while mounted
//   (e.g. disk drives, cartridges)
// * It is up to the mounter to automatically eject a disk image
// * To unmount, the framework sends an empty string
// * To update mount status, use the device status interface
//

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
	string informativeText;
	OEComponent *canvas;
	OEComponent *storage;
} OEEmulationDeviceInfo;

typedef struct
{
	string deviceId;
	OEEmulationDeviceInfo deviceInfo;
} OEEmulationSetDeviceInfo;

typedef void (*OERunAlertCallback)(string message);
typedef OEComponent *(*OEAddCanvasCallback)(void *userData);
typedef void (*OERemoveCanvasCallback)(OEComponent *canvas, void *userData);

typedef map<string, OEEmulationDeviceInfo> OEEmulationDevicesInfo;

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
	void close();
	
	bool setComponent(string id, OEComponent *component);
	
	bool mount(string path);
	bool mount(string deviceId, string path);
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
	
	bool hasProperty(string value, string propertyName);
	string parseProperties(string value, OEPropertiesMap &propertiesMap);
};

#endif
