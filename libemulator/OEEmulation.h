
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
#include "Package.h"

using namespace std;

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
	bool removeOutlet(string outletRef);
	
private:
	bool open;
	xmlDocPtr dml;
	map<string, OEComponent *> components;
	
	Package *package;
	string resourcePath;
	
	string getNodeProperty(xmlNodePtr node, string key);
	void setNodeProperty(xmlNodePtr node, string key, string value);
	
	string buildAbsoluteRef(string absoluteRef, string componentName);
	string buildSourcePath(string deviceName, string src);
	
	bool readResource(string localPath, vector<char> &data);
	
	bool buildComponents();
	bool queryComponents();
	void destructComponents();
	
	bool buildComponent(string deviceName, xmlNodePtr componentNode);
	bool queryComponent(string deviceName, xmlNodePtr componentNode);
	bool initComponent(string deviceName, xmlNodePtr componentNode);
	
	bool connectComponent(string deviceName,
						  OEComponent *component,
						  xmlNodePtr connectionNode);
	bool setComponentProperty(OEComponent *component, xmlNodePtr propertyNode);
	bool getComponentProperty(OEComponent *component, xmlNodePtr propertyNode);
	bool setComponentData(string deviceName,
						  OEComponent *component,
						  xmlNodePtr dataNode);
	bool getComponentData(string deviceName,
						  OEComponent *component,
						  xmlNodePtr dataNode);
	bool setComponentResource(OEComponent *component, xmlNodePtr resourceNode);
};

#endif
