
/**
 * libemulation
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEComponentFactory.h"
#include "StorageInterface.h"

OEEmulation::OEEmulation() : OEEDL()
{
	runAlert = NULL;
	addCanvas = NULL;
	removeCanvas = NULL;
	devicesDidUpdate = NULL;
	
	setComponent("emulation", this);
}

OEEmulation::~OEEmulation()
{
	close();
	
	disconnectEmulation();
	destroyEmulation();
}

void OEEmulation::setResourcePath(string path)
{
	resourcePath = path;
}

void OEEmulation::setRunAlertCallback(OERunAlertCallback runAlert)
{
	this->runAlert = runAlert;
}

void OEEmulation::setAddCanvasCallback(OEAddCanvasCallback addCanvas,
									   void *userData)
{
	this->addCanvas = addCanvas;
	addCanvasUserData = userData;
}

void OEEmulation::setRemoveCanvasCallback(OERemoveCanvasCallback removeCanvas,
										  void *userData)
{
	this->addCanvas = addCanvas;
	addCanvasUserData = userData;
}

void OEEmulation::setDevicesDidUpdateCallback(OEDevicesDidUpdateCallback
											  devicesDidUpdate)
{
	this->devicesDidUpdate = devicesDidUpdate;
}

bool OEEmulation::open(string path)
{
	if (!OEEDL::open(path))
		return false;
	
	buildDevicesInfo();
	
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
					log("could not open '" + path + "'");
			}
			else
				log("could not dump EDL for '" + path + "'");
		}
		else
			log("could not update the configuration for '" + path + "'");
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
						log("could not write '" OE_PACKAGE_EDL_PATH
							"' in '" + path + "'");
				}
				else
					log("could not dump EDL for '" + path + "'");
			}
			
			delete package;
			package = NULL;
		}
		else
			log("could not open '" + path + "'");
	}
	else
		log("could not identify type of '" + path + "'");
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEmulation::setComponent(string id, OEComponent *component)
{
	if (component)
		componentsMap[id] = component;
	else
		componentsMap.erase(id);
	
	return true;
}

OEComponent *OEEmulation::getComponent(string id)
{
	if (!componentsMap.count(id))
		return NULL;
	
	return componentsMap[id];
}

bool OEEmulation::addEDL(string path, OEIdMap connectionMap)
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



void OEEmulation::buildDevicesInfo()
{
	devicesInfo.clear();
	
	if (!doc)
		return;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			OEDeviceInfo deviceInfo;
			deviceInfo.id = getNodeProperty(node, "id");
			deviceInfo.group = getNodeProperty(node, "group");
			deviceInfo.label = getNodeProperty(node, "label");
			deviceInfo.image = getNodeProperty(node, "image");
			deviceInfo.settings = buildDeviceSettings(node->children);
			
			deviceInfo.location = buildDeviceLocation(deviceInfo.id);
			
			deviceInfo.powered = false;
			devicesInfo.push_back(deviceInfo);
		}
	}
}

OESettings OEEmulation::buildDeviceSettings(xmlNodePtr children)
{
	OESettings settings;
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "setting"))
		{
			OESetting setting;
			
			setting.ref = getNodeProperty(node, "ref");
			setting.name = getNodeProperty(node, "name");
			setting.type = getNodeProperty(node, "type");
			setting.options = getNodeProperty(node, "options");
			setting.label = getNodeProperty(node, "label");
			
			settings.push_back(setting);
		}
	}
	
	return settings;
}

string OEEmulation::buildDeviceLocation(string deviceId)
{
	if (!doc)
		return "";
	
	vector<string> visitedDevices;
	string location = buildDeviceLocation(deviceId, visitedDevices);
	int depth = visitedDevices.size() - 1;
	
	return depth ? location : "";
}

string OEEmulation::buildDeviceLocation(string deviceId,
										vector<string> &visitedDevices)
{
	if (!doc)
		return "";
	
	// Circularity check
	if (find(visitedDevices.begin(), visitedDevices.end(), deviceId) !=
		visitedDevices.end())
		return "*";
	visitedDevices.push_back(deviceId);
	
	// Find connected port
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "port"))
		{
			string id = getNodeProperty(node, "id");
			string ref = getNodeProperty(node, "ref");
			string label = getNodeProperty(node, "label");
			
			if (getDeviceId(ref) == deviceId)
				return (buildDeviceLocation(getDeviceId(id), visitedDevices) +
						" " + label);
		}
	}
	
	// Find device label
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			string label = getNodeProperty(node, "label");
			
			if (deviceId == id)
				return label;
		}
	}
	
	// Device not found
	return "?";
}

OEDevicesInfo &OEEmulation::getDevicesInfo()
{
	return devicesInfo;
}

OEDeviceInfo *OEEmulation::getDeviceInfo(string id)
{
	for (OEDevicesInfo::iterator i = devicesInfo.begin();
		 i != devicesInfo.end();
		 i++)
	{
		if (i->id == id)
			return &(*i);
	}
	
	return NULL;
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
			return setComponent(id, component);
		else
			log("could not create '" + id + "', class '" + className + "' undefined");
	}
	else
		log("redefinition of '" + id + "'");
	
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
		log("could not configure '" + id + "', it was not created");
		return false;
	}
	
	OEPropertiesMap propertiesMap;
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
					log("invalid property '" + name + "' for '" + id + "'");
			}
			else if (hasNodeProperty(node, "ref"))
			{
				string refId = getNodeProperty(node, "ref");
				OEComponent *ref = getComponent(refId);
				if (!component->setRef(name, ref))
					log("invalid property '" + name + "' for '" + id + "'");
			}
			else if (hasNodeProperty(node, "src"))
			{
				string src = getNodeProperty(node, "src");
				
				OEData *data = new OEData;
				string parsedSrc = parseValueProperties(src, propertiesMap);
				bool dataRead = false;
				if (hasValueProperty(src, "packagePath"))
				{
					if (package)
						dataRead = package->readFile(parsedSrc, data);
				}
				else
					dataRead = readFile(parsedSrc, data);
				
				if (!dataRead)
				{
					delete data;
					data = NULL;
				}
				
				if (!component->setData(name, data))
					log("invalid property '" + name + "' for '" + id + "'");
			}
			else
			{
				log("invalid property '" + name + "' for '" + id +
					"', unrecognized type");
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
		log("could not init '" + id + "', it was not created");
		return false;
	}
	
	if (component->init())
		return true;
	else
		log("could not init '" + id + "'");
	
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
		log("could not update '" + id + "', it was not created");
		return false;
	}
	
	OEPropertiesMap propertiesMap;
	propertiesMap["id"] = id;
	propertiesMap["device"] = getDeviceId(id);
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
					setNodeProperty(node, name, value);
			}
			else if (hasNodeProperty(node, "src"))
			{
				string src = getNodeProperty(node, "src");
				
				if (!hasValueProperty(src, "packagePath") || !package)
					continue;
				
				OEData *data = NULL;
				string parsedSrc = parseValueProperties(src, propertiesMap);
				
				if (component->getData(name, &data) && data)
				{
					if (!package->writeFile(parsedSrc, data))
						log("could not write '" + src + "'");
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
	
	setComponent(id, NULL);
}

bool OEEmulation::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case OEEMULATION_SET_STATE:
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->state = *((string *)data);
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_SET_IMAGE:
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->image = *((string *)data);
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_SET_POWERED:
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->powered = *((bool *)data);
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_ADD_CANVAS:
			if (data && addCanvas)
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					
					*ref = addCanvas(addCanvasUserData);
					
					deviceInfo->canvases.push_back(*ref);
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_REMOVE_CANVAS:
			if (data && removeCanvas)
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					
					OEComponents &canvases = deviceInfo->canvases;
					OEComponents::iterator first = canvases.begin();
					OEComponents::iterator last = canvases.end();
					OEComponents::iterator i = remove(first, last, *ref);
					if (i != last)
						canvases.erase(i, last);
					
					removeCanvas(*ref, removeCanvasUserData);
					
					*ref = NULL;
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_ADD_STORAGE:
			if (data && addCanvas)
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					
					deviceInfo->storages.push_back(*ref);
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_REMOVE_STORAGE:
			if (data && removeCanvas)
			{
				OEDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					
					OEComponents &storages = deviceInfo->storages;
					OEComponents::iterator first = storages.begin();
					OEComponents::iterator last = storages.end();
					OEComponents::iterator i = remove(first, last, *ref);
					if (i != last)
						storages.erase(i, last);
					
					*ref = NULL;
					
					return postMessage(sender, OEEMULATION_UPDATE_DEVICES, NULL);
				}
			}
			break;
			
		case OEEMULATION_RUN_ALERT:
			if (data && runAlert)
			{
				string *message = (string *)data;
				if (runAlert)
					runAlert(*message);
				
				return true;
			}
			break;
			
		case OEEMULATION_UPDATE_DEVICES:
			if (devicesDidUpdate)
				devicesDidUpdate();
			
			return true;
			
		case OEEMULATION_MOUNT:
			return OEComponent::postMessage(sender, STORAGE_MOUNT, data);

		case OEEMULATION_IS_MOUNTABLE:
			return OEComponent::postMessage(sender, STORAGE_IS_MOUNTABLE, data);
			
		case OEEMULATION_GET_MOUNT_PATH:
			return OEComponent::postMessage(sender, STORAGE_GET_MOUNT_PATH, data);
			
		case OEEMULATION_IS_LOCKED:
			return OEComponent::postMessage(sender, STORAGE_IS_LOCKED, data);
	}
	
	return false;
}

bool OEEmulation::hasValueProperty(string value, string propertyName)
{
	return (value.find("${" + propertyName + "}") != string::npos);
}

string OEEmulation::parseValueProperties(string value,
										 OEPropertiesMap &propertiesMap)
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
		
		for (OEPropertiesMap::iterator i = propertiesMap.begin();
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

void OEEmulation::setDeviceId(string &id, string deviceId)
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

string OEEmulation::getDeviceId(OEComponent *component)
{
	for (OEComponentsMap::iterator i = componentsMap.begin();
		 i != componentsMap.end();
		 i++)
	{
		if (i->second == component)
			return getDeviceId(i->first);
	}
	
	return "";
}

/*
bool OEEmulation::isDevice(string deviceId)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			
			if (getDeviceId(id) == deviceId)
				return true;
		}
	
	return false;
}

OEIdList OEEmulation::getDeviceIds()
{
	OEIdList deviceIds;
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			
			deviceIds.push_back(id);
		}
	
	return deviceIds;
}

OEIdMap OEEDL::buildNameMap(OEIdList deviceIds, OEIdList newDeviceIds)
{
	OEIdMap nameMap;
	
	for (OEIdList::iterator i = newDeviceIds.begin();
		 i != newDeviceIds.end();
		 i++)
	{
		string deviceId = *i;
		string newDeviceId = deviceId;
		int newDeviceIdCount = 1;
		
		while (find(deviceIds.begin(), deviceIds.end(), newDeviceId) !=
			   deviceIds.end())
		{
			newDeviceIdCount++;
			newDeviceId = deviceId + "_" + getString(newDeviceIdCount);
		}
		
		nameMap[deviceId] = newDeviceId;
	}
	
	return nameMap;
}

void OEEDL::rename(OEIdMap nameMap)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		rename(nameMap, node, "id");
		rename(nameMap, node, "ref");
		
		if (node->children)
		{
			for(xmlNodePtr propertyNode = node->children;
				propertyNode;
				propertyNode = propertyNode->next)
			{
				rename(nameMap, propertyNode, "id");
				rename(nameMap, propertyNode, "ref");
			}
		}
	}
}

void OEEDL::rename(OEIdMap nameMap, xmlNodePtr node, string property)
{
	string nodeProperty;
	
	if (hasNodeProperty(node, "id"))
		nodeProperty = "id";
	else if (hasNodeProperty(node, "ref"))
		nodeProperty = "ref";
	
	if (nodeProperty != "")
	{
		string id = getNodeProperty(node, nodeProperty);
		
		string deviceId = getDeviceId(id);
		if (nameMap.count(deviceId))
		{
			setDeviceId(id, nameMap[deviceId]);
			setNodeProperty(node, nodeProperty, id);
		}
	}
}

bool OEEDL::renameConnectionMap(OEIdMap &connectionMap, OEIdMap nameMap)
{
	for (OEIdMap::iterator i = connectionMap.begin();
		 i != connectionMap.end();
		 i++)
	{
		string portId = i->first;
		string connectorId = i->second;
		
		string deviceId = getDeviceId(connectorId);
		if (!nameMap.count(deviceId))
			return false;
		setDeviceId(connectorId, nameMap[deviceId]);
		
		connectionMap[portId] = connectorId;
	}
	
	return true;
}

void OEEDL::insert(OEEDL *edl, string deviceId)
{
	// Idea
	// - Buscar el punto de inserción (buscar el device del primer port,
	// y seleccionar el punto antes del proximo device
	// - Insertar los elementos que matcheen con algun device conectado
	// - Iterar sobre los ports, luego sobre los inlets, y definir referencias
	// - Iterar sobre los conectores, luego sobre los inlets, y definir referencias
	//
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string id = getNodeProperty(node, "id");
			if (getDeviceId(id) == deviceId)
				setNodeProperty(node, "ref", "");
			
		}
		for(xmlNodePtr propertyNode = node->children;
			propertyNode;
			propertyNode = propertyNode->next)
		{
			string ref = getNodeProperty(propertyNode, "ref");
			
			if (getDeviceId(ref) == deviceId)
				setNodeProperty(propertyNode, "ref", "");
		}
	}
	
	
}

void OEEDL::connect()
{
	
	
}

bool OEEDL::removeConnectedDevices(string deviceId)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "port"))
		{
			string id = getNodeProperty(node, "id");
			if (getDeviceId(id) == deviceId)
			{
				string ref = getNodeProperty(node, "ref");
				
				if (!removeDevice(getDeviceId(ref)))
					return false;
			}
		}
	}
	
	return true;
}

void OEEDL::disconnect(string deviceId)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		string ref = getNodeProperty(node, "ref");
		if (getDeviceId(ref) == deviceId)
			setNodeProperty(node, "ref", "");
		
		for(xmlNodePtr propertyNode = node->children;
			propertyNode;
			propertyNode = propertyNode->next)
		{
			string ref = getNodeProperty(propertyNode, "ref");
			
			if (getDeviceId(ref) == deviceId)
				setNodeProperty(propertyNode, "ref", "");
		}
	}
}

void OEEDL::removeElements(string deviceId)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (getDeviceId(getNodeProperty(node, "id")) == deviceId)
		{
			xmlNodePtr nextNode = node->next;
			if (nextNode && (nextNode->type == XML_TEXT_NODE))
			{
                xmlUnlinkNode(nextNode);
				xmlFreeNode(nextNode);
			}
			
			xmlUnlinkNode(node);
			xmlFreeNode(node);
		}
	}
}

bool OEInfo::addEDL(string path, OEIdMap &deviceIdMap)
{
	if (!OEEDL::addEDL(path, deviceIdMap))
		return false;
	
	return analyze();
}

bool OEInfo::removeDevice(string deviceName)
{
	if (!OEEDL::removeDevice(deviceName))
		return false;
	
	return analyze();
}
*/
