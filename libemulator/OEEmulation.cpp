
/**
 * libemulator
 * Emulation
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEComponentFactory.h"

OEEmulation::OEEmulation(string emulationPath, string resourcePath)
{
	documentDML = NULL;
	open = false;
	
	this->resourcePath = resourcePath;
	package = new OEPackage(emulationPath);
	if (package->isOpen())
	{
		vector<char> data;
		if (package->readFile(OE_DML_FILENAME, data))
		{
			documentDML = xmlReadMemory(&data[0],
										data.size(),
										OE_DML_FILENAME,
										NULL,
										0);
			
			constructDML(documentDML);
			if (initDML(documentDML))
				open = true;
		}
	}
	
	delete package;
	package = NULL;
}

OEEmulation::~OEEmulation()
{
	destroyDML(documentDML);
	
	if (documentDML)
		xmlFreeDoc(documentDML);
}

bool OEEmulation::readFile(string path, vector<char> &data)
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
		error = !file.good();
		file.close();
	}
	
	return !error;
}

string OEEmulation::getXMLProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEEmulation::setXMLProperty(xmlNodePtr node, string key, string value)
{
	xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
}

string OEEmulation::buildSourcePath(string src, OERef deviceRef)
{
	string deviceName = deviceRef.getStringRef();
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

bool OEEmulation::validateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(node, "version") == "1.0");
}

bool OEEmulation::constructDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!constructDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::initDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!initDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::reconnectDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!reconnectDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::updateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		updateDevice(childNode);
	}
	
	return true;
}

void OEEmulation::destroyDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		destroyDevice(childNode);
	}
}

bool OEEmulation::constructDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!constructComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::initDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!initComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::reconnectDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!reconnectComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::updateDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!updateComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

void OEEmulation::destroyDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		destroyComponent(childNode, deviceRef);
	}
}

bool OEEmulation::constructComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentClass = getXMLProperty(node, "class");
	string componentName = getXMLProperty(node, "name");
	
	OEComponent *component = OEComponentFactory::build(string(componentClass));
	
	if (component)
	{
		string componentRef = deviceRef.buildRef(componentName).getStringRef();
//		printf("OEEmulation::constructComponent: %s\n", componentRef.c_str());
		
		if (components[componentRef])
			delete components[componentRef];
		
		components[componentRef] = component;
	}
	
	return (component != NULL);
}

bool OEEmulation::initComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.buildRef(componentName).getStringRef();
	OEComponent *component = components[componentRef];
	
	//	printf("OEEmulation::initComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "connection"))
		{
			if (!setConnection(childNode, component, deviceRef))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!setProperty(childNode, component))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
		{
			if (!setData(childNode, component, deviceRef))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "resource"))
		{
			if (!setResource(childNode, component))
				return false;
		}
	}
	
	return true;
}

bool OEEmulation::reconnectComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.buildRef(componentName).getStringRef();
	OEComponent *component = components[componentRef];
	
	//	printf("OEEmulation::initComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "connection"))
		{
			if (!setConnection(childNode, component, deviceRef))
				return false;
		}
	}
	
	return true;
}

bool OEEmulation::updateComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.buildRef(componentName).getStringRef();
	OEComponent *component = components[componentRef];
	
	//	printf("OEEmulation::queryComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!getProperty(childNode, component))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
		{
			if (!getData(childNode, component, deviceRef))
				return false;
		}
	}
	
	return true;
}

void OEEmulation::destroyComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.buildRef(componentName).getStringRef();
	
	delete components[componentRef];
	components.erase(componentRef);
}

bool OEEmulation::setConnection(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	OEComponent *connectedComponent;
	
	string key = getXMLProperty(node, "key");
	string ref = getXMLProperty(node, "ref");
	
	if (!ref.size())
		connectedComponent = NULL;
	else
	{
		string componentRef = deviceRef.buildRef(ref).getStringRef();
		connectedComponent = components[componentRef];
		
		if(!connectedComponent)
		{
			cerr << "libemulator: could not connect \"" + componentRef + "\"." << endl;
			return false;
		}
	}
	
	OEIoctlConnection msg;
	msg.key = key;
	msg.component = connectedComponent;
	
	component->ioctl(OEIoctlSetConnection, &msg);
	
	return true;
}

bool OEEmulation::setProperty(xmlNodePtr node, OEComponent *component)
{
	string key = getXMLProperty(node, "key");
	string value = getXMLProperty(node, "value");
	
	OEIoctlProperty msg;
	msg.key = key;
	msg.value = value;
	
	component->ioctl(OEIoctlSetProperty, &msg);
	
	return true;
}

bool OEEmulation::getProperty(xmlNodePtr node, OEComponent *component)
{
	string key = getXMLProperty(node, "key");
	
	OEIoctlProperty msg;
	msg.key = key;
	
	if (component->ioctl(OEIoctlGetProperty, &msg))
		setXMLProperty(node, "value", msg.value);
	
	return true;
}

bool OEEmulation::setData(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	string key = getXMLProperty(node, "key");
	string src = buildSourcePath(getXMLProperty(node, "src"), deviceRef);
	
	OEIoctlData msg;
	msg.key = key;
	
	if (package->readFile(src, msg.data))
		component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

bool OEEmulation::getData(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	string key = getXMLProperty(node, "key");
	string src = buildSourcePath(getXMLProperty(node, "src"), deviceRef);
	
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

bool OEEmulation::setResource(xmlNodePtr node, OEComponent *component)
{
	string key = getXMLProperty(node, "key");
	string src = getXMLProperty(node, "src");
	
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

bool OEEmulation::isOpen()
{
	return open;
}

bool OEEmulation::save(string emulationPath)
{
	bool error = true;
	
	package = new OEPackage(emulationPath);
	if (package->isOpen())
	{
		if (open && updateDML(documentDML))
		{
			xmlChar *datap;
			int size;
			xmlDocDumpMemory(documentDML, &datap, &size);
			
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
	return documentDML;
}

bool OEEmulation::isDeviceNameInDML(xmlDocPtr doc, string deviceName)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (getXMLProperty(childNode, "name") == deviceName)
			return true;
	}
	
	return false;
}

void OEEmulation::buildDeviceNameMap(xmlDocPtr deviceDML, map<string, string> &deviceNameMap)
{
	xmlNodePtr node = xmlDocGetRootElement(deviceDML);
	int devIndex = 0;
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getXMLProperty(childNode, "name");
		stringstream ss;
		ss << devIndex;
		string newDeviceName = string("dev") + ss.str();
		while (isDeviceNameInDML(documentDML, newDeviceName))
			devIndex++;
		deviceNameMap[deviceName] = newDeviceName;
	}
}

void OEEmulation::renameDMLRefs(xmlDocPtr doc, map<string, string> &deviceNameMap)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = deviceNameMap[getXMLProperty(childNode, "name")];
		setXMLProperty(childNode, "name", deviceName);
		OERef deviceRef(deviceName);
		
		for(xmlNodePtr componentNode = childNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			string componentName = getXMLProperty(componentNode, "name");
			string componentRef = deviceRef.buildRef(componentName).getStringRef();
			
			for(xmlNodePtr node = componentNode->children;
				node;
				node = node->next)
			{
				if (!xmlStrcmp(node->name, BAD_CAST "connection"))
				{
					OERef ref = deviceRef.buildRef(getXMLProperty(node, "ref"));
					string deviceName = ref.getDevice();
					
					if (deviceNameMap.count(deviceName) > 0)
					{
						ref.setDevice(deviceNameMap[deviceName]);
						setXMLProperty(node, "ref", ref.getStringRef());
					}
				}
			}
		}
	}
}

void OEEmulation::renameConnections(map<string, string> &connections,
									map<string, string> &deviceNameMap)
{
	for (map<string, string>::iterator i = connections.begin();
		 i != connections.end();
		 i++)
		connections[i->first] = deviceNameMap[i->second];
}

void OEEmulation::insertDML(xmlDocPtr deviceDML, xmlDocPtr documentDML, string insertBeforeRef)
{
	// Search inlet and previous busy inlet
	xmlNodePtr node = xmlDocGetRootElement(deviceDML);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
	}
	
	return true;
	// If found, select the device pointed by the inlet
	// If not found, select the current device
	// Insert after selected device
}

bool OEEmulation::addDML(string path, map<string, string> connections)
{
	vector<char> data;
	if (!readFile(path, data))
		return false;
	xmlDocPtr deviceDML;
	deviceDML = xmlReadMemory(&data[0],
							  data.size(),
							  OE_DML_FILENAME,
							  NULL,
							  0);
	if (!deviceDML)
		return false;
	if (!validateDML(deviceDML))
	{
		xmlFreeDoc(deviceDML);
		return false;
	}
	
	map<string, string> deviceNameMap;
	buildDeviceNameMap(deviceDML, deviceNameMap);
	
	renameDMLRefs(deviceDML, deviceNameMap);
	renameConnections(connections, deviceNameMap);

	insertDML(deviceDML, documentDML, "");
	
	constructDML(deviceDML);
	
	reconnectDML(documentDML);
	
	return false;
}

bool OEEmulation::removeOutlet(OERef outletRef)
{
	// Search device with outletRef
	// Find device's inlets
	// Iterate removal for every connected device
	// Remove component of device
	
	return false;
}
