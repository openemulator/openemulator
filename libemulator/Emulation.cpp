
/**
 * libemulator
 * Emulation
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <fstream>
#include <libxml/parser.h>

#include "Emulation.h"
#include "ComponentFactory.h"

#define COMPONENT_DATA_DEVICE_NAME "${DEVICE_NAME}"

Emulation::Emulation(string emulationPath, string resourcePath)
{
	dml = NULL;
	isEmulationLoaded = false;
	
	this->resourcePath = resourcePath;
	package = new Package(emulationPath, false);
	if (package->isOpen())
	{
		vector<char> data;
		if (package->readFile(DMLINFO_FILENAME, data))
		{
			dml = xmlReadMemory(&data[0],
								data.size(),
								DMLINFO_FILENAME,
								NULL,
								0);
			
			isEmulationLoaded = buildComponents();
		}
	}
	
	delete package;
	package = NULL;
}

Emulation::~Emulation()
{
	if (dml)
		xmlFreeDoc(dml);
	
	destroyComponents();
}

string Emulation::getNodeProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = string(value);
	xmlFree(value);
	
	return valueString;
}

void Emulation::setNodeProperty(xmlNodePtr node, string key, string value)
{
	xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
}

string Emulation::buildComponentRef(string deviceName, string componentName)
{
	if (componentName.size() == 0)
		return string();
	
	if (componentName.find("::") == string::npos)
		componentName = deviceName + "::" + componentName;
	
	return componentName;
}

string Emulation::buildSourcePath(string deviceName, string src)
{
	// Replace COMPONENT_DATA_DEVICE_NAME with deviceName
	int index = src.find(COMPONENT_DATA_DEVICE_NAME);
	if (index != string::npos)
		src.replace(index, sizeof(COMPONENT_DATA_DEVICE_NAME) - 1, deviceName);
	
	return src;
}

bool Emulation::readFile(string path, vector<char> &data)
{
	bool error = true;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data.resize(size);
		file.read(&data[0], size);
		error = file.failbit;
		file.close();
	}
	
	return !error;
}

bool Emulation::buildComponents()
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	// Instantiate components
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		string deviceName = (char *)deviceNode->name;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (!xmlStrcmp(componentNode->name, BAD_CAST "component"))
				if (!buildComponent(deviceName, componentNode))
					return false;
		}
	}
	
	// Send ioctl messages
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		string deviceName = (char *) deviceNode->name;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (!xmlStrcmp(componentNode->name, BAD_CAST "component"))
			{
				if (!initComponent(deviceName, componentNode))
					return false;
			}
		}
	}
	
	return true;
}

bool Emulation::buildComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentClass = getNodeProperty(componentNode, "class");
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildComponentRef(deviceName, string(componentName));
	Component *component = ComponentFactory::build(string(componentClass));
	
	if (component)
		components[componentRef] = component;
	
	return (component != NULL);
}

void Emulation::destroyComponents()
{
	map<string, Component *>::iterator iterator;
	for (iterator = components.begin();
		 iterator != components.end();
		 iterator++)
	{
		delete iterator->second;
		components.erase(iterator);
	}
}

bool Emulation::initComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildComponentRef(deviceName, string(componentName));
	Component *component = components[componentRef];
	
	for(xmlNodePtr propertyNode = componentNode->children;
		propertyNode;
		propertyNode = propertyNode->next)
	{
		if (xmlStrcmp(propertyNode->name, BAD_CAST "connection"))
		{
			if (!connectComponent(deviceName, component, propertyNode))
				return false;
		}
		else if (xmlStrcmp(propertyNode->name, BAD_CAST "property"))
		{
			if (!setComponentProperty(component, propertyNode))
				return false;
		}
		else if (xmlStrcmp(propertyNode->name, BAD_CAST "data"))
		{
			if (!setComponentData(deviceName, component, propertyNode))
				return false;
		}
		else if (xmlStrcmp(propertyNode->name, BAD_CAST "resource"))
		{
			if (!setComponentResource(component, propertyNode))
				return false;
		}
	}
	
	return true;
}

bool Emulation::connectComponent(string deviceName,
								 Component *component,
								 xmlNodePtr connectionNode)
{
	string key = getNodeProperty(connectionNode, "key");
	string ref = getNodeProperty(connectionNode, "ref");
	
	string componentRef = buildComponentRef(deviceName, ref);
	Component *connectedComponent = components[componentRef];
	
	if(!connectedComponent)
		return false;
	
	struct IOCTLConnection ioctl;
	ioctl.key = key;
	ioctl.component = connectedComponent;
	
	component->ioctl(IOCTL_SETCONNECTION, &ioctl);
	
	return true;
}

bool Emulation::setComponentProperty(Component *component,
									  xmlNodePtr propertyNode)
{
	string key = getNodeProperty(propertyNode, "key");
	string value = getNodeProperty(propertyNode, "value");
	
	struct IOCTLProperty ioctl;
	ioctl.key = key;
	ioctl.value = value;
	
	component->ioctl(IOCTL_SETPROPERTY, &ioctl);
	
	return true;
}

bool Emulation::setComponentData(string deviceName,
								  Component *component,
								  xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName,
								 getNodeProperty(dataNode, "src"));
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	ioctl.data = vector<char>();
	
	if (!package->readFile(src, ioctl.data));
		return false;
	
	component->ioctl(IOCTL_SETDATA, &ioctl);
	
	return true;
}	

bool Emulation::setComponentResource(Component *component,
									  xmlNodePtr resourceNode)
{
	string key = getNodeProperty(resourceNode, "key");
	string src = getNodeProperty(resourceNode, "src");
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	ioctl.data = vector<char>();
	
	if (!readFile(resourcePath + src, ioctl.data))
		return false;
	
	component->ioctl(IOCTL_SETDATA, &ioctl);
	
	return true;
}	

bool Emulation::isOpen()
{
	return isEmulationLoaded;
}

bool Emulation::queryComponents()
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	// Send ioctl messages
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		string deviceName = (char *) deviceNode->name;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (!xmlStrcmp(componentNode->name, BAD_CAST "component"))
			{
				if (!queryComponent(deviceName, componentNode))
					return false;
			}
		}
	}
	
	return true;
}

bool Emulation::queryComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildComponentRef(deviceName, string(componentName));
	Component *component = components[componentRef];
	
	for(xmlNodePtr propertyNode = componentNode->children;
		propertyNode;
		propertyNode = propertyNode->next)
	{
		if (xmlStrcmp(propertyNode->name, BAD_CAST "property"))
		{
			if (!getComponentProperty(component, propertyNode))
				return false;
		}
		else if (xmlStrcmp(propertyNode->name, BAD_CAST "data"))
		{
			if (!getComponentData(deviceName, component, propertyNode))
				return false;
		}
	}
	
	return true;
}

bool Emulation::getComponentProperty(Component *component,
									 xmlNodePtr propertyNode)
{
	string key = getNodeProperty(propertyNode, "key");
	
	struct IOCTLProperty ioctl;
	ioctl.key = key;
	ioctl.value = string();
	
	component->ioctl(IOCTL_GETPROPERTY, &ioctl);
	
	setNodeProperty(propertyNode, "value", ioctl.value);
	
	return true;
}

bool Emulation::getComponentData(string deviceName,
								 Component *component,
								 xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName, 
								 getNodeProperty(dataNode, "src"));
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	ioctl.data = vector<char>();
	
	component->ioctl(IOCTL_GETDATA, &ioctl);
	
	if (!package->writeFile(src, ioctl.data));
		return false;
	
	return true;
}

bool Emulation::save(string emulationPath)
{
	bool error = true;
	
	if (queryComponents())
	{
		package = new Package(emulationPath, true);
		if (package->isOpen())
		{
			xmlIndentTreeOutput = 1;
			
			xmlChar *datap;
			int size;
			xmlDocDumpFormatMemory(dml, &datap, &size, 1);

			if (datap)
			{
				vector<char> data(size);
				memcpy(&data[0], datap, size);
				
				xmlFree(datap);
				
				error = !package->writeFile(DMLINFO_FILENAME, data);
			}
		}
	}
	
	return !error;
}

int Emulation::ioctl(string componentRef, int message, void *data)
{
	Component *component = components[componentRef];
	
	if (component)
		return component->ioctl(message, data);
	
	return 0;
}

xmlDocPtr Emulation::getDML()
{
	return dml;
}

bool Emulation::addDML(string path, map<string, string> connections)
{
	// Load DML Tree
	// Rename device names, so they don't step on old ones
	// Update reference names in tree and conn
	// Add new tree to old one
	// Go over the connections, and set the inlets to the outlets
	return false;
}

bool Emulation::removeOutlet(string outletRef)
{
	// Search component with outletRef
	// Get components' inlets
	// Iterate from the beginning with all connected outlets
	return false;
}
