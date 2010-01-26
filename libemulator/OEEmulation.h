
/**
 * libemulator
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

#include <libxml/tree.h>

#include "OEDefines.h"
#include "OEComponent.h"
#include "OERef.h"
#include "OEPackage.h"

using namespace std;

typedef map<string, OEComponent *> OEComponentsMap;
typedef map<string, string> OEStringRefMap;

class OEEmulation
{
public:
	OEEmulation(string emulationPath, string resourcePath);
	~OEEmulation();
	
	bool isOpen();
	
	bool save(string emulationPath);
	
	int ioctl(string ref, int message, void *data);
	
	xmlDocPtr getDML();
	bool addDML(string path, OEStringRefMap connections);
	bool removeDevicesFromOutletRef(OERef ref);
	
private:
	bool open;
	xmlDocPtr documentDML;
	OEComponentsMap components;
	
	OEPackage *package;
	string resourcePath;
	
	string toString(int i);
	bool readFile(string path, vector<char> &data);
	
	string getXMLProperty(xmlNodePtr node, string name);
	void setXMLProperty(xmlNodePtr node, string name, string value);
	
	string buildSourcePath(string src, OERef deviceRef);
	
	bool readResource(string localPath, vector<char> &data);
	
	bool validateDML(xmlDocPtr doc);
	bool constructDML(xmlDocPtr doc);
	bool initDML(xmlDocPtr doc);
	bool reconnectDML(xmlDocPtr doc);
	bool updateDML(xmlDocPtr doc);
	void destroyDML(xmlDocPtr doc);
	
	bool constructDevice(xmlNodePtr node);
	bool initDevice(xmlNodePtr node);
	bool reconnectDevice(xmlNodePtr node);
	bool updateDevice(xmlNodePtr node);
	void destroyDevice(xmlNodePtr node);
	
	bool constructComponent(xmlNodePtr node, OERef deviceRef);
	bool initComponent(xmlNodePtr node, OERef deviceRef);
	bool reconnectComponent(xmlNodePtr node, OERef deviceRef);
	bool updateComponent(xmlNodePtr node, OERef deviceRef);
	void destroyComponent(xmlNodePtr node, OERef deviceRef);
	
	bool setConnection(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	bool setProperty(xmlNodePtr node, OEComponent *component);
	bool getProperty(xmlNodePtr node, OEComponent *component);
	bool setData(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	bool getData(xmlNodePtr node, OEComponent *component, OERef deviceRef);
	bool setResource(xmlNodePtr node, OEComponent *component);
	
	xmlNodePtr getNodeForRef(xmlDocPtr doc, OERef ref);
	OERef getOutletRefForInletRef(xmlDocPtr doc, OERef ref);
	
	void buildDeviceNameMap(xmlDocPtr doc, xmlDocPtr elem, OEStringRefMap &deviceNameMap);
	void renameDMLRefs(xmlDocPtr doc, OEStringRefMap &deviceNameMap);
	void renameConnections(OEStringRefMap &connections, OEStringRefMap &deviceNameMap);
	xmlNodePtr getInsertionPointForInletRef(xmlDocPtr doc, OERef ref);
	bool mergeDMLs(xmlDocPtr doc, xmlDocPtr elem, OEStringRefMap &connections);
};

#endif
