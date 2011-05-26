
/**
 * libemulation
 * OEEmulation
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation.
 */

#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEDevice.h"

#include "OEComponentFactory.h"

OEEmulation::OEEmulation() : OEEDL()
{
	createCanvas = NULL;
	destroyCanvas = NULL;
	didUpdate = NULL;
	
	activityCount = 0;
	
	addComponent("emulation", this);
}

OEEmulation::~OEEmulation()
{
	close();
	
	didUpdate = NULL;
	
	disconnectEmulation();
	destroyEmulation();
}

void OEEmulation::setResourcePath(string path)
{
	resourcePath = path;
}

void OEEmulation::setCreateCanvas(EmulationCreateCanvas createCanvas)
{
	this->createCanvas = createCanvas;
}

void OEEmulation::setDestroyCanvas(EmulationDestroyCanvas destroyCanvas)
{
	this->destroyCanvas = destroyCanvas;
}

void OEEmulation::setDidUpdate(EmulationDidUpdate didUpdate)
{
	this->didUpdate = didUpdate;
}

void OEEmulation::setUserData(void *userData)
{
	this->userData = userData;
}

bool OEEmulation::open(string path)
{
	if (!OEEDL::open(path))
		return false;
	
	if (createEmulation())
		if (configureEmulation())
			if (initEmulation())
				return true;
	
	close();
	
	return false;
}

bool OEEmulation::save(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_FILE_PATH_EXTENSION)
	{
		if (updateEmulation())
		{
			if (dumpEmulation(&data))
			{
				is_open = writeFile(path, &data);
				
				if (!is_open)
					printLog("could not open '" + path + "'");
			}
			else
				printLog("could not dump EDL for '" + path + "'");
		}
		else
			printLog("could not update the configuration for '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage();
		if (package && package->open(path))
		{
			if (updateEmulation())
			{
				if (dumpEmulation(&data))
				{
					is_open = package->writeFile(OE_PACKAGE_EDL_PATH, &data);
					if (!is_open)
						printLog("could not write '" OE_PACKAGE_EDL_PATH
								   "' in '" + path + "'");
				}
				else
					printLog("could not dump EDL for '" + path + "'");
			}
			
			delete package;
			package = NULL;
		}
		else
			printLog("could not open '" + path + "'");
	}
	else
		printLog("could not identify type of '" + path + "'");
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEmulation::addComponent(string id, OEComponent *component)
{
	if (!component)
		return false;
	
	if (componentsMap.count(id))
		return false;
	
	componentsMap[id] = component;
	
	return true;
}

bool OEEmulation::removeComponent(string id)
{
	if (componentsMap.count(id))
		return false;
	
	componentsMap.erase(id);
	
	return true;
}

OEComponent *OEEmulation::getComponent(string id)
{
	if (!componentsMap.count(id))
		return NULL;
	
	return componentsMap[id];
}

string OEEmulation::getId(OEComponent *component)
{
	for (map<string, OEComponent *>::iterator i = componentsMap.begin();
		 i != componentsMap.end();
		 i++)
	{
		if (i->second == component)
			return i->first;
	}
	
	return "";
}

OEIds *OEEmulation::getDevices()
{
	return &devices;
}

bool OEEmulation::addEDL(string path, map<string, string> connectionMap)
{
	if (!doc)
		return false;
	
	// Load new EDL
	OEEDL edl;
	if (!edl.open(path))
		return false;
	
/*	// Build name map for new EDL
	OEIdList deviceIds = getDeviceIds();
	OEIdList newDeviceIds = edl.getDeviceIds();
	OEIdMap nameMap = buildNameMap(deviceIds, newDeviceIds);
	
	// Rename EDL id's, and rename connection map
	edl.renameEmulation(nameMap);
	renameConnectionMap(connectionMap, nameMap);
	
	// Insert EDL
	//	xmlNodePtr insertionNode = findInsertionPoint(connectionMap.begin()->first)
	//	insert(insertionNode, &edl);
	
	// Connect port inlets
	connectEmulation();
	*/
	return true;
}

bool OEEmulation::removeDevice(string id)
{
	for (OEIds::iterator i = devices.begin();
		 i != devices.end();
		 i++)
	{
		if (*i == id)
		{
			devices.erase(i);
			return true;
		}
	}
	
	return false;
	// Verify device exists
/*	if (!isDevice(deviceId))
	{
		log("could not find '" + deviceId + "'");
		return false;
	}
	
	// Recursively remove devices connected to this device
	if (!removeConnectedDevices(deviceId))
		return false;
	
	// Remove references to this device
	disconnectDevice(deviceId);
	
	// Remove elements matching this device
	removeElements(deviceId);
	*/
	return true;
}

bool OEEmulation::isActive()
{
	return (activityCount != 0);
}



bool OEEmulation::dumpEmulation(OEData *data)
{
	if (!doc)
		return false;
	
	xmlChar *xmlDump;
	int xmlDumpSize;
	
	xmlDocDumpMemory(doc, &xmlDump, &xmlDumpSize);
	if (xmlDump)
	{
		data->resize(xmlDumpSize);
		memcpy(&data->front(), xmlDump, xmlDumpSize);
		
		xmlFree(xmlDump);
		
		return true;
	}
	
	return false;
}

bool OEEmulation::createEmulation()
{
	if (!doc)
		return false;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!createDevice(id))
				return false;
		}
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			string className = getNodeProperty(node, "class");
			
			if (!createComponent(id, className))
				return false;
		}
	}
	
	return true;
}

bool OEEmulation::createDevice(string id)
{
	if (!getComponent(id))
	{
		OEComponent *component;
		component = new OEDevice(this);
		
		if (component)
		{
			if (addComponent(id, component))
			{
				devices.push_back(id);
				return true;
			}
		}
		else
			printLog("could not create device '" + id + "'");
	}
	else
		printLog("redefinition of '" + id + "'");
	
	return false;
}

bool OEEmulation::createComponent(string id, string className)
{
	if (!getComponent(id))
	{
		OEComponent *component;
		component = OEComponentFactory::create(className);
		
		if (component)
			return addComponent(id, component);
		else
			printLog("could not create '" + id +
					   "', class '" + className + "' undefined");
	}
	else
		printLog("redefinition of '" + id + "'");
	
	return false;
}

bool OEEmulation::configureEmulation()
{
	if (!doc)
		return false;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			string label = getNodeProperty(node, "label");
			string image = getNodeProperty(node, "image");
			string group = getNodeProperty(node, "group");
			
			if (!configureDevice(id, label, image, group, node->children))
				return false;
		}
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!configureComponent(id, node->children))
				return false;
		}
	}
	
	return true;
}

bool OEEmulation::configureDevice(string id,
								  string label, string image, string group, 
								  xmlNodePtr children)
{
	OEComponent *device = getComponent(id);
	string locationLabel = getLocationLabel(id);
	
	// Parse settings
	DeviceSettings settings;
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "setting"))
		{
			DeviceSetting setting;
			
			setting.ref = getNodeProperty(node, "ref");
			setting.name = getNodeProperty(node, "name");
			setting.type = getNodeProperty(node, "type");
			setting.options = getNodeProperty(node, "options");
			setting.label = getNodeProperty(node, "label");
			
			settings.push_back(setting);
		}
	}
	
	device->postMessage(this, DEVICE_SET_LABEL, &label);
	device->postMessage(this, DEVICE_SET_IMAGEPATH, &image);
	device->postMessage(this, DEVICE_SET_GROUP, &group);
	device->postMessage(this, DEVICE_SET_LOCATIONLABEL, &locationLabel);
	device->postMessage(this, DEVICE_SET_SETTINGS, &settings);
	
	return true;
}

bool OEEmulation::configureComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		printLog("could not configure '" + id + "', it is not created");
		return false;
	}
	
	map<string, string> propertiesMap;
	propertiesMap["id"] = id;
	propertiesMap["deviceId"] = getDeviceId(id);
	propertiesMap["resourcePath"] = resourcePath;
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "value"))
			{
				string value = getNodeProperty(node, "value");
				value = parseValueProperties(value, propertiesMap);
				
				if (!component->setValue(name, value))
					printLog("'" + id + "': invalid property '" + name + "'");
			}
			else if (hasNodeProperty(node, "ref"))
			{
				string refId = getNodeProperty(node, "ref");
				OEComponent *ref = getComponent(refId);
				if (!component->setRef(name, ref))
					printLog("'" + id + "': invalid property '" + name + "'");
			}
			else if (hasNodeProperty(node, "data"))
			{
				string dataSrc = getNodeProperty(node, "data");
				
				OEData data;
				string parsedSrc = parseValueProperties(dataSrc, propertiesMap);
				bool dataRead = false;
				if (hasValueProperty(dataSrc, "packagePath"))
				{
					if (package)
						dataRead = package->readFile(parsedSrc, &data);
				}
				else
					dataRead = readFile(parsedSrc, &data);
				
				if (dataRead)
				{
					if (!component->setData(name, &data))
						printLog("'" + id + "': invalid property '" + name + "'");
				}
			}
			else
			{
				printLog("'" + id + "': invalid property '" + name + "', "
						   "unrecognized type");
			}
		}
	}
	
	return true;
}

bool OEEmulation::initEmulation()
{
	if (!doc)
		return false;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!initComponent(id))
				return false;
		}
	
	return true;
}

bool OEEmulation::initComponent(string id)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		printLog("could not init '" + id + "', it is not created");
		
		return false;
	}
	
	if (!component->init())
	{
		printLog("could not init '" + id + "'");
		
		return false;
	}
	
	component->update();
	
	return true;
}

bool OEEmulation::updateEmulation()
{
	if (!doc)
		return false;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!updateComponent(id, node->children))
				return false;
		}
	
	return true;
}

bool OEEmulation::updateComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		printLog("could not update '" + id + "', it is not created");
		return false;
	}
	
	map<string, string> propertiesMap;
	propertiesMap["id"] = id;
	propertiesMap["deviceId"] = getDeviceId(id);
	propertiesMap["resourcePath"] = resourcePath;
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "value"))
			{
				string value;
				
				if (component->getValue(name, value))
					setNodeProperty(node, "value", value);
			}
			else if (hasNodeProperty(node, "data"))
			{
				string dataSrc = getNodeProperty(node, "data");
				
				if (!hasValueProperty(dataSrc, "packagePath") || !package)
					continue;
				
				OEData *data = NULL;
				string parsedSrc = parseValueProperties(dataSrc, propertiesMap);
				
				if (component->getData(name, &data) && data)
				{
					if (!package->writeFile(parsedSrc, data))
						printLog("could not write '" + dataSrc + "'");
				}
			}
		}
	}
	
	return true;
}

void OEEmulation::disconnectEmulation()
{
	if (!doc)
		return;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			disconnectComponent(id, node->children);
		}
	}
}

void OEEmulation::disconnectComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
		return;
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "ref"))
				component->setRef(name, NULL);
		}
	}
}

void OEEmulation::destroyEmulation()
{
	if (!doc)
		return;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			destroyComponent(id, node->children);
		}
	}
}

void OEEmulation::destroyComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
		return;
	
	delete component;
	
	removeComponent(id);
}

bool OEEmulation::hasValueProperty(string value, string propertyName)
{
	return (value.find("${" + propertyName + "}") != string::npos);
}

string OEEmulation::parseValueProperties(string value, map<string, string>& propertiesMap)
{
	int startIndex;
	
	while ((startIndex = value.find("${")) != string::npos)
	{
		int endIndex = value.find("}", startIndex);
		if (endIndex == string::npos)
		{
			value = value.substr(0, startIndex);
			break;
		}
		
		string propertyName = value.substr(startIndex + 2,
										   endIndex - startIndex - 2);
		string replacement;
		
		for (map<string, string>::iterator i = propertiesMap.begin();
			 i != propertiesMap.end();
			 i++)
		{
			if (i->first == propertyName)
				replacement = i->second;
		}
		
		value = value.replace(startIndex,
							  endIndex - startIndex + 1,
							  replacement);
	}
	
	return value;
}

string OEEmulation::getLocationLabel(string id)
{
	if (!doc)
		return "";
	
	vector<string> visitedIds;
	string location = getLocationLabel(id, visitedIds);
	int depth = visitedIds.size();
	
	if (depth == 1)
		return "";
	else
		return location;
}

string OEEmulation::getLocationLabel(string id,
									 vector<string>& visitedIds)
{
	if (!doc)
		return "";
	
	// Circularity check
	if (find(visitedIds.begin(), visitedIds.end(), id) !=
		visitedIds.end())
		return "*";
	visitedIds.push_back(id);
	
	// Find connected port
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "port"))
		{
			string portId = getNodeProperty(node, "id");
			string portRef = getNodeProperty(node, "ref");
			string portLabel = getNodeProperty(node, "label");
			
			if (getDeviceId(portRef) == id)
				return (getLocationLabel(getDeviceId(portId), visitedIds) +
						" " + portLabel);
		}
	}
	
	// Find device label
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string deviceId = getNodeProperty(node, "id");
			string deviceLabel = getNodeProperty(node, "label");
			
			if (deviceId == id)
				return deviceLabel;
		}
	}
	
	// Device not found
	return "?";
}
