
/**
 * libemulator
 * OEEmulator
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <iostream>
#include <fstream>

#include <libxml/parser.h>

#include "OEEmulator.h"
#include "OEComponentFactory.h"

OEEmulator::OEEmulator(string emulationPath, string resourcePath)
{
//	printf("OEEmulator::construct\n");
	dml = NULL;
	open = false;
	
	this->resourcePath = resourcePath;
	package = new Package(emulationPath);
	if (package->isOpen())
	{
		vector<char> data;
		if (package->readFile(OE_DML_FILENAME, data))
		{
			dml = xmlReadMemory(&data[0],
								data.size(),
								OE_DML_FILENAME,
								NULL,
								0);
			
			if (buildComponents())
				open = true;
		}
	}
	
	delete package;
	package = NULL;
}

OEEmulator::~OEEmulator()
{
//	printf("OEEmulator::destruct\n");
	if (dml)
		xmlFreeDoc(dml);
	
	destructComponents();
}

string OEEmulator::getNodeProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEEmulator::setNodeProperty(xmlNodePtr node, string key, string value)
{
	xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
}

string OEEmulator::buildAbsoluteRef(string absoluteRef, string ref)
{
	if (ref.size() == 0)
		return string();
	
	if (ref.find("::") == string::npos)
	{
		int index = absoluteRef.find("::");
		if (index != string::npos)
			absoluteRef = absoluteRef.substr(index);
		
		ref = absoluteRef + "::" + ref;
	}
	
	return ref;
}

string OEEmulator::buildSourcePath(string deviceName, string src)
{
	// Replace COMPONENT_DATA_DEVICE_NAME with deviceName
	int index = src.find(OE_SUBSTITUTION_DEVICE_NAME);
	if (index != string::npos)
		src.replace(index, sizeof(OE_SUBSTITUTION_DEVICE_NAME) - 1, deviceName);
	
	return src;
}

bool OEEmulator::readResource(string localPath, vector<char> &data)
{
	bool error = true;
	
	ifstream file((resourcePath + OE_PATH_SEPARATOR + localPath).c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data.resize(size);
		file.read(&data[0], size);
		error = !file.good();
		file.close();
	}
	
	return !error;
}

bool OEEmulator::buildComponents()
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	if (getNodeProperty(dmlNode, "version") != "1.0")
		return false;
	
	// Build components
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getNodeProperty(deviceNode, "name");
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (!buildComponent(deviceName, componentNode))
				return false;
		}
	}
	
	// Init components
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getNodeProperty(deviceNode, "name");
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (!initComponent(deviceName, componentNode))
				return false;
		}
	}
	
	return true;
}

bool OEEmulator::queryComponents()
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	// Query components
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getNodeProperty(deviceNode, "name");
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (!queryComponent(deviceName, componentNode))
				return false;
		}
	}
	
	return true;
}

void OEEmulator::destructComponents()
{
	for (map<string, OEComponent *>::iterator i = components.begin();
		 i != components.end();
		 i++)
	{
//		printf("OEEmulator::destructComponent: %s\n", i->first.c_str());
		
		delete i->second;
		components.erase(i);
	}
}

bool OEEmulator::buildComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentClass = getNodeProperty(componentNode, "class");
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = OEComponentFactory::build(string(componentClass));
	
//	printf("OEEmulator::buildComponent: %s\n", componentRef.c_str());
	
	if (component)
	{
		if (components[componentRef])
			delete components[componentRef];
		
		components[componentRef] = component;
	}
	
	return (component != NULL);
}

bool OEEmulator::initComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = components[componentRef];
	
//	printf("OEEmulator::initComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr node = componentNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "connection"))
		{
			if (!connectComponent(deviceName, component, node))
				return false;
		}
		else if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			if (!setComponentProperty(component, node))
				return false;
		}
		else if (!xmlStrcmp(node->name, BAD_CAST "data"))
		{
			if (!setComponentData(deviceName, component, node))
				return false;
		}
		else if (!xmlStrcmp(node->name, BAD_CAST "resource"))
		{
			if (!setComponentResource(component, node))
				return false;
		}
	}
	
	return true;
}

bool OEEmulator::queryComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = components[componentRef];
	
//	printf("OEEmulator::queryComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr node = componentNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			if (!getComponentProperty(component, node))
				return false;
		}
		else if (!xmlStrcmp(node->name, BAD_CAST "data"))
		{
			if (!getComponentData(deviceName, component, node))
				return false;
		}
	}
	
	return true;
}

bool OEEmulator::connectComponent(string deviceName,
								   OEComponent *component,
								   xmlNodePtr connectionNode)
{
	string key = getNodeProperty(connectionNode, "key");
	string ref = getNodeProperty(connectionNode, "ref");
	
	if (!ref.size())
		return true;
	
	string componentRef = buildAbsoluteRef(deviceName, ref);
	OEComponent *connectedComponent = components[componentRef];
	
	if(!connectedComponent)
	{
		cerr << "libemulator: could not connect \"" + componentRef + "\"." << endl;
		return false;
	}
	
	OEIoctlConnection msg;
	msg.key = key;
	msg.component = connectedComponent;
	
	component->ioctl(OEIoctlSetConnection, &msg);
	
	return true;
}

bool OEEmulator::setComponentProperty(OEComponent *component,
									   xmlNodePtr propertyNode)
{
	string key = getNodeProperty(propertyNode, "key");
	string value = getNodeProperty(propertyNode, "value");
	
	OEIoctlProperty msg;
	msg.key = key;
	msg.value = value;
	
	component->ioctl(OEIoctlSetProperty, &msg);
	
	return true;
}

bool OEEmulator::getComponentProperty(OEComponent *component,
									   xmlNodePtr propertyNode)
{
	string key = getNodeProperty(propertyNode, "key");
	
	OEIoctlProperty msg;
	msg.key = key;
	
	if (component->ioctl(OEIoctlGetProperty, &msg))
		setNodeProperty(propertyNode, "value", msg.value);
	
	return true;
}

bool OEEmulator::setComponentData(string deviceName,
								   OEComponent *component,
								   xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName,
								 getNodeProperty(dataNode, "src"));
	
	OEIoctlData msg;
	msg.key = key;
	
	if (package->readFile(src, msg.data))
		component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

bool OEEmulator::getComponentData(string deviceName,
								   OEComponent *component,
								   xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName, 
								 getNodeProperty(dataNode, "src"));
	
	OEIoctlData msg;
	msg.key = key;
	
	if (component->ioctl(OEIoctlGetData, &msg))
	{
		if (!package->writeFile(src, msg.data))
		{
			cerr << "libemulator: could not write \"" + src + "\"." << endl;
			return false;
		}
	}
	
	return true;
}

bool OEEmulator::setComponentResource(OEComponent *component,
									   xmlNodePtr resourceNode)
{
	string key = getNodeProperty(resourceNode, "key");
	string src = getNodeProperty(resourceNode, "src");
	
	OEIoctlData msg;
	msg.key = key;
	
	if (!readResource(src, msg.data))
	{
		cerr << "libemulator: could not read \"" + src + "\"." << endl;
		return false;
	}
	
	component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

bool OEEmulator::isOpen()
{
	return open;
}

bool OEEmulator::save(string emulationPath)
{
	bool error = true;
	
	package = new Package(emulationPath);
	if (package->isOpen())
	{
		if (open && queryComponents())
		{
			xmlChar *datap;
			int size;
			xmlDocDumpMemory(dml, &datap, &size);
			
			if (datap)
			{
				vector<char> data(size);
				memcpy(&data[0], datap, size);
				
				xmlFree(datap);
				
				error = !package->writeFile(OE_DML_FILENAME, data);
			}
		}
		
		if (error)
			package->remove();
	}
	
	return !error;
}

int OEEmulator::ioctl(string componentRef, int message, void *data)
{
	if (open)
	{
		OEComponent *component = components[componentRef];
		
		if (component)
			return component->ioctl(message, data);
	}
	
	return 0;
}

xmlDocPtr OEEmulator::getDML()
{
	return dml;
}

bool OEEmulator::addDML(string path, map<string, string> connections)
{
	// Load DML Tree
	// Rename device names, so they don't step on old ones
	// Update reference names in tree and conn
	// Add new tree to old one
	// Go over the connections, and set the inlets to the outlets
	return false;
}

bool OEEmulator::removeOutlet(string outletRef)
{
	// Search component with outletRef
	// Get components' inlets
	// Iterate from the beginning with all connected outlets
	return false;
}
