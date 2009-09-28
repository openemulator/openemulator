
/**
 * libemulator
 * OEEmulation
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <fstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEComponentFactory.h"

OEEmulation::OEEmulation(string emulationPath, string resourcePath)
{
	printf("constructor\n");
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

OEEmulation::~OEEmulation()
{
	printf("destructor\n");
	if (dml)
		xmlFreeDoc(dml);
	
	destructComponents();
}

string OEEmulation::getNodeProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEEmulation::setNodeProperty(xmlNodePtr node, string key, string value)
{
	xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
}

string OEEmulation::buildAbsoluteRef(string absoluteRef, string ref)
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

string OEEmulation::buildSourcePath(string deviceName, string src)
{
	// Replace COMPONENT_DATA_DEVICE_NAME with deviceName
	int index = src.find(OE_SUBSTITUTION_DEVICE_NAME);
	if (index != string::npos)
		src.replace(index, sizeof(OE_SUBSTITUTION_DEVICE_NAME) - 1, deviceName);
	
	return src;
}

bool OEEmulation::readResource(string localPath, vector<char> &data)
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

bool OEEmulation::buildComponents()
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

bool OEEmulation::queryComponents()
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

void OEEmulation::destructComponents()
{
	for (map<string, OEComponent *>::iterator i = components.begin();
		 i != components.end();
		 i++)
	{
		printf("destructComponent: %s\n", i->first.c_str());
		
		delete i->second;
		components.erase(i);
	}
}

bool OEEmulation::buildComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentClass = getNodeProperty(componentNode, "class");
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = OEComponentFactory::build(string(componentClass));
	
	printf("buildComponent: %s\n", componentRef.c_str());
	
	if (component)
	{
		if (components[componentRef])
			delete components[componentRef];
		
		components[componentRef] = component;
	}
	
	return (component != NULL);
}

bool OEEmulation::initComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = components[componentRef];
	
	printf("initComponent: %s\n", componentRef.c_str());
	
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

bool OEEmulation::queryComponent(string deviceName, xmlNodePtr componentNode)
{
	string componentName = getNodeProperty(componentNode, "name");
	
	string componentRef = buildAbsoluteRef(deviceName, componentName);
	OEComponent *component = components[componentRef];
	
	printf("queryComponent: %s\n", componentRef.c_str());
	
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

bool OEEmulation::connectComponent(string deviceName,
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
		return false;
	
	struct IOCTLConnection ioctl;
	ioctl.key = key;
	ioctl.component = connectedComponent;
	
	component->ioctl(IOCTL_SETCONNECTION, &ioctl);
	
	return true;
}

bool OEEmulation::setComponentProperty(OEComponent *component,
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

bool OEEmulation::getComponentProperty(OEComponent *component,
									   xmlNodePtr propertyNode)
{
	string key = getNodeProperty(propertyNode, "key");
	
	struct IOCTLProperty ioctl;
	ioctl.key = key;
	
	if (component->ioctl(IOCTL_GETPROPERTY, &ioctl))
		setNodeProperty(propertyNode, "value", ioctl.value);
	
	return true;
}

bool OEEmulation::setComponentData(string deviceName,
								   OEComponent *component,
								   xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName,
								 getNodeProperty(dataNode, "src"));
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	
	if (package->readFile(src, ioctl.data))
		component->ioctl(IOCTL_SETDATA, &ioctl);
	
	return true;
}

bool OEEmulation::getComponentData(string deviceName,
								   OEComponent *component,
								   xmlNodePtr dataNode)
{
	string key = getNodeProperty(dataNode, "key");
	string src = buildSourcePath(deviceName, 
								 getNodeProperty(dataNode, "src"));
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	
	if (component->ioctl(IOCTL_GETDATA, &ioctl))
	{
		if (!package->writeFile(src, ioctl.data))
			return false;
	}
	
	return true;
}

bool OEEmulation::setComponentResource(OEComponent *component,
									   xmlNodePtr resourceNode)
{
	string key = getNodeProperty(resourceNode, "key");
	string src = getNodeProperty(resourceNode, "src");
	
	struct IOCTLData ioctl;
	ioctl.key = key;
	ioctl.data = vector<char>();
	
	if (!readResource(src, ioctl.data))
		return false;
	
	component->ioctl(IOCTL_SETDATA, &ioctl);
	
	return true;
}

bool OEEmulation::isOpen()
{
	return open;
}

bool OEEmulation::save(string emulationPath)
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

int OEEmulation::ioctl(string componentRef, int message, void *data)
{
	if (open)
	{
		OEComponent *component = components[componentRef];
		
		if (component)
			return component->ioctl(message, data);
	}
	
	return 0;
}

xmlDocPtr OEEmulation::getDML()
{
	return dml;
}

bool OEEmulation::addDML(string path, map<string, string> connections)
{
	// Load DML Tree
	// Rename device names, so they don't step on old ones
	// Update reference names in tree and conn
	// Add new tree to old one
	// Go over the connections, and set the inlets to the outlets
	return false;
}

bool OEEmulation::removeOutlet(string outletRef)
{
	// Search component with outletRef
	// Get components' inlets
	// Iterate from the beginning with all connected outlets
	return false;
}
