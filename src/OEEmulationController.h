
/**
 * OpenEmulator
 * OpenEmulator emulation controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator emulation controller object.
 */

#ifndef _OEEMULATIONCONTROLLER_H
#define _OEEMULATIONCONTROLLER_H

#include "HostEmulationControllerInterface.h"

typedef void (*OERunAlertCallback)(string message);
typedef void (*OEAddCanvasCallback)(OEComponent *canvas, void *userData);
typedef void (*OERemoveCanvasCallback)(OEComponent *canvas, void *userData);

typedef map<string, HostEmulationControllerDeviceInfo> OEEmulationControllerDevicesInfo;

class OEEmulationController: public OEComponent
{
public:
	OEEmulationController();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
	void setAlertCallback(OERunAlertCallback alertCallback);
	void setAddCanvasCallback(OEAddCanvasCallback addCanvasCallback,
							  void *userData);
	void setRemoveCanvasCallback(OERemoveCanvasCallback removeCanvasCallback,
								 void *userData);
	bool mount(string path);
	bool mount(string deviceId, string path);
	bool validate(string path);
	
private:
	OEEmulationControllerDevicesInfo devicesInfo;
	
	OERunAlertCallback alertCallback;
	OEAddCanvasCallback addCanvasCallback;
	OERemoveCanvasCallback removeCanvasCallback;
	void *addCanvasCallbackUserData;
	void *removeCanvasCallbackUserData;
};

#endif
