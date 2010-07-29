
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

typedef map<string, OEComponent *> OEComponents;

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
	
	bool add(string path, OEConnections &connections);
	bool remove(string ref);
	
protected:
	
private:
	string resourcePath;
	OEComponents components;
	
	bool setComponent(string ref, OEComponent *component);
	
	bool construct();
	bool constructDevice(xmlNodePtr node);
	bool constructComponent(xmlNodePtr node, string deviceRef);
	
	bool init();
	bool initDevice(xmlNodePtr node);
	bool initComponent(xmlNodePtr node, string deviceRef);
	
	bool connect();
	bool connectDevice(xmlNodePtr node);
	bool connectComponent(xmlNodePtr node, string deviceRef);
	
	void update();
	bool updateDevice(xmlNodePtr node);
	bool updateComponent(xmlNodePtr node, string deviceRef);
	
	void destroy();
	bool destroyDevice(xmlNodePtr node);
	void destroyComponent(xmlNodePtr node, string deviceRef);
	
	bool setProperty(xmlNodePtr node, OEComponent *component);
	bool getProperty(xmlNodePtr node, OEComponent *component);
	bool setData(xmlNodePtr node, OEComponent *component, string ref);
	bool getData(xmlNodePtr node, OEComponent *component, string ref);
	bool setResource(xmlNodePtr node, OEComponent *component);
	bool connect(xmlNodePtr node, OEComponent *component, string ref);
};

#endif
