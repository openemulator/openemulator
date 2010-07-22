
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

typedef map<string, OEComponent *> OEComponentsMap;
typedef map<string, string> OEStringRefMap;

class OEEmulation : public OEInfo
{
public:
	OEEmulation(string path);
	OEEmulation(string path, string resourcePath);
	~OEEmulation();
	
	bool open(string path);
	void close();
	
	OEComponent *getComponent(string ref);
	
	bool addDevices(string path, OEStringRefMap connections);
	bool removeDevice(OERef ref);
	
private:
	string resourcePath;
	OEComponentsMap components;
	
	bool iterate(bool (OEEmulation::*callback)(xmlNodePtr node));
	bool iterateDevice(xmlNodePtr node,
					   bool (OEEmulation::*callback)(xmlNodePtr node, OERef ref));
	
	bool constructDevice(xmlNodePtr node);
	bool initDevice(xmlNodePtr node);
	bool connectDevice(xmlNodePtr node);
	bool updateDevice(xmlNodePtr node);
	bool destroyDevice(xmlNodePtr node);
	
	bool constructComponent(xmlNodePtr node, OERef deviceRef);
	bool initComponent(xmlNodePtr node, OERef deviceRef);
	bool connectComponent(xmlNodePtr node, OERef deviceRef);
	bool updateComponent(xmlNodePtr node, OERef deviceRef);
	void destroyComponent(xmlNodePtr node, OERef deviceRef);
	
	bool setProperty(xmlNodePtr node, OEComponent *component);
	bool getProperty(xmlNodePtr node, OEComponent *component);
	bool setData(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	bool getData(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	bool setResource(xmlNodePtr node, OEComponent *component);
	bool setConnection(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	
	xmlNodePtr getNodeOfFirstInlet(xmlDocPtr doc, OERef ref);
	xmlNodePtr getNodeOfLastInlet(xmlDocPtr doc,
								  OERef ref,
								  vector<OERef> &visitedRefs);
	xmlNodePtr getNodeOfPreviousInlet(xmlDocPtr doc, OERef ref);
	
	void buildDeviceNameMap(xmlDocPtr doc,
							xmlDocPtr elem,
							OEStringRefMap &deviceNameMap);
	void renameDMLConnections(xmlDocPtr doc,
							  OEStringRefMap &connections,
							  OEStringRefMap &deviceNameMap);
	
	bool connectDevices(xmlDocPtr doc, OEStringRefMap &connections);
	bool disconnectDevice(xmlDocPtr doc, OERef ref);
	
	bool insertDoc(xmlNodePtr node, xmlDocPtr doc);
	
	string buildSourcePath(string src, OERef deviceRef);
};

#endif
