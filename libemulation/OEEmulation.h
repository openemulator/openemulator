
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

#include <string>
#include <map>
#include <iostream>

#include <libxml/tree.h>

#include "OERef.h"
#include "OEComponent.h"
#include "OEPackage.h"

#define OE_REF_SUBST_STRING "${REF}"

using namespace std;

typedef map<string, OEComponent *> OEComponentsMap;
typedef map<string, string> OEStringRefMap;

class OEEmulation
{
public:
	OEEmulation(string path, string resourcePath);
	~OEEmulation();
	
	bool isLoaded();
	
	bool save(string path);
	
	OEComponent *getComponent(string ref);
	
	xmlDocPtr getDML();
	bool addDevices(string path, OEStringRefMap connections);
	bool isDeviceTerminal(OERef ref);
	bool removeDevice(OERef ref);
	
private:
	bool loaded;
	xmlDocPtr documentDML;
	OEComponentsMap components;
	
	OEPackage *package;
	string resourcePath;
	
	string getString(int value);
	
	bool readFile(string path, vector<char> &data);
	string buildSourcePath(string src, OERef deviceRef);
	
	string getXMLProperty(xmlNodePtr node, string name);
	void setXMLProperty(xmlNodePtr node, string name, string value);
	
	bool validateDML(xmlDocPtr doc);
	bool constructDML(xmlDocPtr doc);
	bool initDML(xmlDocPtr doc);
	bool connectDML(xmlDocPtr doc);
	bool updateDML(xmlDocPtr doc);
	void destroyDML(xmlDocPtr doc);
	
	bool constructDevice(xmlNodePtr node);
	bool initDevice(xmlNodePtr node);
	bool connectDevice(xmlNodePtr node);
	bool updateDevice(xmlNodePtr node);
	void destroyDevice(xmlNodePtr node);
	
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
	
	xmlNodePtr getNodeForRef(xmlDocPtr doc, OERef ref);
	OERef getOutletForInlet(xmlDocPtr doc, OERef ref);
	xmlNodePtr getNodeOfFirstInlet(xmlDocPtr, OERef ref);
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
};

#endif
