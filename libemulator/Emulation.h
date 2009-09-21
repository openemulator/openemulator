
/**
 * libemulator
 * Emulation
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _EMULATION_H
#define _EMULATION_H

#include <string>
#include <map>

#include <libxml/tree.h>

#include "Component.h"
#include "Package.h"

#define DMLINFO_FILENAME "info.xml"

using namespace std;

class Emulation
{
public:
	Emulation(string emulationPath, string resourcePath);
	~Emulation();
	
	bool isOpen();
	bool save(string emulationPath);
	int ioctl(string componentRef, int message, void *data);
	
	xmlDocPtr getDML();
	bool addDML(string dmlPath, map<string, string> connections);
	bool removeOutlet(string outletRef);
	
private:
	bool isEmulationLoaded;
	xmlDocPtr dml;
	map<string, Component *> components;
	
	Package *package;
	string resourcePath;
	
	string getNodeProperty(xmlNodePtr node, string key);
	void setNodeProperty(xmlNodePtr node, string key, string value);
	
	string buildComponentRef(string deviceName, string componentName);
	string buildSourcePath(string deviceName, string src);
	
	bool buildComponents();
	bool buildComponent(string deviceName, xmlNodePtr componentNode);
	bool initComponent(string deviceName, xmlNodePtr componentNode);
	bool connectComponent(string deviceName,
						  Component *component,
						  xmlNodePtr connectionNode);
	bool setComponentProperty(Component *component, xmlNodePtr propertyNode);
	bool setComponentData(string deviceName,
						   Component *component,
						   xmlNodePtr dataNode);
	bool setComponentResource(Component *component, xmlNodePtr resourceNode);
	
	bool queryComponents();
	bool queryComponent(string deviceName, xmlNodePtr componentNode);
	bool getComponentProperty(Component *component, xmlNodePtr propertyNode);
	bool getComponentData(string deviceName,
						  Component *component,
						  xmlNodePtr dataNode);
};

#endif
