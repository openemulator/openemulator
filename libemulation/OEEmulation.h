
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

#include <map>

#include "OEInfo.h"
#include "OEComponent.h"

#define OE_REF_SUBST_STRING "${REF}"

typedef map<string, OEComponent *> OEComponents;
typedef map<string, string> OEConnections;

class OEEmulation : public OEInfo
{
public:
	OEEmulation(string path);
	OEEmulation(string path, string resourcePath);
	~OEEmulation();
	
	bool open(string path);
	void close();
	
	OEComponent *getComponent(string ref);
	
	bool addDML(string path, OEConnections connections);
	bool removeDevice(string ref);
	
private:
	string resourcePath;
	OEComponents components;
	
	bool construct();
	bool init();
	bool connect();
	void update();
	void destroy();
	
	bool constructDevice(xmlNodePtr node);
	bool initDevice(xmlNodePtr node);
	bool connectDevice(xmlNodePtr node);
	bool updateDevice(xmlNodePtr node);
	bool destroyDevice(xmlNodePtr node);
	
	bool constructComponent(xmlNodePtr node, string deviceRef);
	bool initComponent(xmlNodePtr node, string deviceRef);
	bool connectComponent(xmlNodePtr node, string deviceRef);
	bool updateComponent(xmlNodePtr node, string deviceRef);
	void destroyComponent(xmlNodePtr node, string deviceRef);
	
	bool setProperty(xmlNodePtr node, OEComponent *component);
	bool getProperty(xmlNodePtr node, OEComponent *component);
	bool setData(xmlNodePtr node, OEComponent *component, string deviceRef);
	bool getData(xmlNodePtr node, OEComponent *component, string deviceRef);
	bool setResource(xmlNodePtr node, OEComponent *component);
	bool setConnection(xmlNodePtr node, OEComponent *component, string deviceRef);
	
	xmlNodePtr getNodeOfFirstInlet(xmlDocPtr doc, string ref);
	xmlNodePtr getNodeOfLastInlet(xmlDocPtr doc,
								  string ref,
								  vector<string> &refs);
	xmlNodePtr getNodeOfPreviousInlet(xmlDocPtr doc, string ref);
	
	void buildDeviceNameMap(xmlDocPtr doc,
							xmlDocPtr elem,
							OEConnections &deviceNameMap);
	void renameDMLConnections(xmlDocPtr doc,
							  OEConnections &connections,
							  OEConnections &deviceNameMap);
	
	bool connectDevices(xmlDocPtr doc, map<string, string> &connections);
	bool disconnectDevice(xmlDocPtr doc, string ref);
	
	bool insertDoc(xmlNodePtr node, xmlDocPtr doc);
	
	string buildSourcePath(string src, string deviceRef);
};

#endif
