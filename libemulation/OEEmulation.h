
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

typedef map<string, OEComponent *> OEComponentsMap;

class OEEmulation : public OEInfo
{
public:
	~OEEmulation();

	void setResourcePath(string path);
	
	bool open(string path);
	void close();
	
	bool setComponent(string id, OEComponent *component);
	OEComponent *getComponentById(string id);
	string getIdByComponent(OEComponent *component);
	string getDeviceByComponent(OEComponent *component);
	
private:
	string resourcePath;
	OEComponentsMap components;
	
	bool create();
	bool createComponent(string id, string className);
	bool configure();
	bool configureComponent(string id, xmlNodePtr children);
	bool init();
	bool initComponent(string id);
	bool update();
	bool updateComponent(string id, xmlNodePtr children);
	void deconfigure();
	void deconfigureComponent(string id, xmlNodePtr children);
	void destroy();
	void destroyComponent(string id, xmlNodePtr children);

	bool hasSrcProperty(string value, string property);
	string parseSrc(string value, string id);
};

#endif
