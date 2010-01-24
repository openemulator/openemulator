
/**
 * libemulator
 * Emulation
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
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

class OEEmulation
{
public:
	OEEmulation(string emulationPath, string resourcePath);
	~OEEmulation();
	
	bool isOpen();
	
	bool save(string emulationPath);
	
	int ioctl(string componentRef, int message, void *data);
	
	xmlDocPtr getDML();
	bool addDML(string dmlPath, map<string, string> connections);
	bool removeOutlet(OERef outletRef);
	
private:
	bool open;
	xmlDocPtr documentDML;
	OEComponentsMap components;
	
	OEPackage *package;
	string resourcePath;
	
	bool readFile(string path, vector<char> &data);
	
	string getXMLProperty(xmlNodePtr node, string key);
	void setXMLProperty(xmlNodePtr node, string key, string value);
	
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

	void buildDeviceNameMap(xmlDocPtr deviceDML, map<string, string> &deviceNameMap);
	void renameDMLRefs(xmlDocPtr doc, map<string, string> &deviceNameMap);
	void renameConnections(map<string, string> &connections, map<string, string> &deviceNameMap);
	void insertDML(xmlDocPtr documentDML, xmlDocPtr deviceDML, string insertRef);
	bool isDeviceNameInDML(xmlDocPtr doc, string deviceName);
};

#endif
