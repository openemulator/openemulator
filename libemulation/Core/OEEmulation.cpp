
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

#include "OEComponentFactory.h"
#include "EmulationInterface.h"

OEEmulation::OEEmulation() : OEEDL()
{
	runAlert = NULL;
	createCanvas = NULL;
	destroyCanvas = NULL;
	didUpdate = NULL;
	
	runningCount = 0;
	
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

void OEEmulation::setRunAlert(EmulationRunAlert runAlert)
{
	this->runAlert = runAlert;
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

OEComponents *OEEmulation::getDevices()
{
	return &devicesMap;
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

bool OEEmulation::removeDevice(string deviceId)
{
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

bool OEEmulation::isRunning()
{
	return (runningCount != 0);
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
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			string className = getNodeProperty(node, "class");
			
			if (!createComponent(id, className))
				return false;
		}
	
	return true;
}

bool OEEmulation::createComponent(string id, string className)
{
	if (!getComponent(id))
	{
		OEComponent *component = OEComponentFactory::create(className);
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
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!configureComponent(id, node->children))
				return false;
		}
	}
	
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
	
	if (component->init())
		return true;
	else
		printLog("could not init '" + id + "'");
	
	return false;
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

bool OEEmulation::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case EMULATION_UPDATE:
			if (didUpdate)
				didUpdate(userData);
			
			return true;
		case EMULATION_RUN_ALERT:
			if (runAlert)
			{
				runAlert(userData, *((string *)data));
				
				return true;
			}
			break;
		case EMULATION_ASSERT_RUNNING:
			runningCount++;
			return true;
		case EMULATION_CLEAR_RUNNING:
			if (runningCount <= 0)
				return false;
			
			runningCount--;
			return true;
		case EMULATION_CREATE_CANVAS:
			if (createCanvas && data)
			{
				OEComponent **ref = (OEComponent **)data;
				*ref = createCanvas(userData, sender);
				return true;
			}
			break;
		case EMULATION_DESTROY_CANVAS:
			if (destroyCanvas)
			{
				OEComponent **ref = (OEComponent**)data;
				destroyCanvas(userData, *ref);
				*ref = NULL;
				return true;
			}
			break;
	}
	
	return false;
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

void OEEmulation::setDeviceId(string& id, string deviceId)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		id = deviceId;
	else
		id = deviceId + "." + id.substr(0, dotIndex + 1);
}

string OEEmulation::getDeviceId(string id)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		return id;
	
	return id.substr(0, dotIndex);
}
