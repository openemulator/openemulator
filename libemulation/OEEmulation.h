
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

#include "OEInfo.h"
#include "OEComponent.h"

#define OE_REPLACE_REF_STRING "${REF}"
#define OE_DEFAULT_RESOURCE_PATH "resources"

typedef map<string, OEComponent *> OEComponentsMap;

class OEEmulation : public OEInfo
{
public:
	OEEmulation();
	OEEmulation(string path);
	OEEmulation(string path, string resourcePath);
	~OEEmulation();
	
	bool open(string path);
	void close();
	
	OEComponent *getComponent(string ref);
	
protected:
	
private:
	string resourcePath;
	OEComponentsMap componentsMap;
	
	bool setComponent(string ref, OEComponent *component);
	
	bool build();
	bool buildDevice(xmlNodePtr deviceNode);
	bool buildComponent(xmlNodePtr componentNode, string deviceName);
	
	bool init();
	bool initDevice(xmlNodePtr deviceNode);
	bool initComponent(xmlNodePtr componentNode, string deviceName);
	
	bool connect();
	bool connectDevice(xmlNodePtr deviceNode);
	bool connectComponent(xmlNodePtr componentNode, string deviceName);
	
	void update();
	void updateDevice(xmlNodePtr deviceNode);
	void updateComponent(xmlNodePtr componentNode, string deviceName);
	
	void remove();
	void removeDevice(xmlNodePtr deviceNode);
	void removeComponent(xmlNodePtr componentNode, string deviceName);
	
	bool setProperty(xmlNodePtr componentNode, OEComponent *component, string ref);
	void getProperty(xmlNodePtr componentNode, OEComponent *component, string ref);
	bool setData(xmlNodePtr componentNode, OEComponent *component, string ref);
	void getData(xmlNodePtr componentNode, OEComponent *component, string ref);
	bool setResource(xmlNodePtr componentNode, OEComponent *component, string ref);
	bool connect(xmlNodePtr componentNode, OEComponent *component, string ref);
	
	string replaceRef(string src, string ref);
};

#endif
