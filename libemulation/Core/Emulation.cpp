
/**
 * libemulation
 * Emulation
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "Emulation.h"
#include "StorageInterface.h"
#include "OEComponentFactory.h"

Emulation::Emulation() : OEEDL()
{
	runAlert = NULL;
	createCanvas = NULL;
	destroyCanvas = NULL;
	didUpdate = NULL;
	
	activeCount = 0;
	
	setComponent("emulation", this);
}

Emulation::~Emulation()
{
	close();
	
	didUpdate = NULL;
	
	disconnectEmulation();
	destroyEmulation();
}

void Emulation::setResourcePath(string path)
{
	resourcePath = path;
}

void Emulation::setRunAlert(EmulationRunAlert runAlert)
{
	this->runAlert = runAlert;
}

void Emulation::setCreateCanvas(EmulationCreateCanvas createCanvas)
{
	this->createCanvas = createCanvas;
}

void Emulation::setDestroyCanvas(EmulationDestroyCanvas destroyCanvas)
{
	this->destroyCanvas = destroyCanvas;
}

void Emulation::setDidUpdate(EmulationDidUpdate didUpdate)
{
	this->didUpdate = didUpdate;
}

void Emulation::setUserData(void *userData)
{
	this->userData = userData;
}

bool Emulation::open(string path)
{
	if (!OEEDL::open(path))
		return false;
	
	buildEmulationInfo();
	
	if (createEmulation())
		if (configureEmulation())
			if (initEmulation())
				return true;
	
	close();
	
	return false;
}

bool Emulation::save(string path)
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

bool Emulation::setComponent(string id, OEComponent *component)
{
	if (component)
		componentsMap[id] = component;
	else
		componentsMap.erase(id);
	
	return true;
}

OEComponent *Emulation::getComponent(string id)
{
	if (!componentsMap.count(id))
		return NULL;
	
	return componentsMap[id];
}

bool Emulation::addEDL(string path, map<string, string> connectionMap)
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

bool Emulation::removeDevice(string deviceId)
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

bool Emulation::isActive()
{
	return (activeCount != 0);
}



void Emulation::buildEmulationInfo()
{
	emulationInfo.clear();
	
	if (!doc)
		return;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			EmulationDeviceInfo deviceInfo;
			deviceInfo.id = getNodeProperty(node, "id");
			deviceInfo.label = getNodeProperty(node, "label");
			deviceInfo.image = getNodeProperty(node, "image");
			deviceInfo.group = getNodeProperty(node, "group");
			if (deviceInfo.group == "")
				deviceInfo.group = "system";
			
			deviceInfo.settings = buildDeviceSettings(node->children);
			
			deviceInfo.location = buildDeviceLocation(deviceInfo.id);
			
			deviceInfo.hotPluggable = false;
			deviceInfo.storage = NULL;
			
			emulationInfo.push_back(deviceInfo);
		}
	}
}

EmulationSettings Emulation::buildDeviceSettings(xmlNodePtr children)
{
	EmulationSettings settings;
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "setting"))
		{
			EmulationSetting setting;
			
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

string Emulation::buildDeviceLocation(string deviceId)
{
	if (!doc)
		return "";
	
	vector<string> visitedDevices;
	string location = buildDeviceLocation(deviceId, visitedDevices);
	int depth = visitedDevices.size() - 1;
	
	return depth ? location : "";
}

string Emulation::buildDeviceLocation(string deviceId,
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

EmulationInfo *Emulation::getEmulationInfo()
{
	return &emulationInfo;
}

EmulationDeviceInfo *Emulation::getDeviceInfo(string id)
{
	for (EmulationInfo::iterator i = emulationInfo.begin();
		 i != emulationInfo.end();
		 i++)
	{
		if (i->id == id)
			return &(*i);
	}
	
	return NULL;
}



bool Emulation::dumpEmulation(OEData *data)
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

bool Emulation::createEmulation()
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

bool Emulation::createComponent(string id, string className)
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

bool Emulation::configureEmulation()
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

bool Emulation::configureComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		log("could not configure '" + id + "', it is not created");
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
					log("'" + id + "': invalid property '" + name + "'");
			}
			else if (hasNodeProperty(node, "ref"))
			{
				string refId = getNodeProperty(node, "ref");
				OEComponent *ref = getComponent(refId);
				if (!component->setRef(name, ref))
					log("'" + id + "': invalid property '" + name + "'");
			}
			else if (hasNodeProperty(node, "data"))
			{
				string dataSrc = getNodeProperty(node, "data");
				
				OEData *data = new OEData;
				string parsedSrc = parseValueProperties(dataSrc, propertiesMap);
				bool dataRead = false;
				if (hasValueProperty(dataSrc, "packagePath"))
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
					log("'" + id + "': invalid property '" + name + "'");
			}
			else
			{
				log("'" + id + "': invalid property '" + name + "', "
					"unrecognized type");
			}
		}
	}
	
	return true;
}

bool Emulation::initEmulation()
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

bool Emulation::initComponent(string id)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		log("could not init '" + id + "', it is not created");
		return false;
	}
	
	if (component->init())
		return true;
	else
		log("could not init '" + id + "'");
	
	return false;
}

bool Emulation::updateEmulation()
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

bool Emulation::updateComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		log("could not update '" + id + "', it is not created");
		return false;
	}
	
	map<string, string> propertiesMap;
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
						log("could not write '" + dataSrc + "'");
				}
			}
		}
	}
	
	return true;
}

void Emulation::disconnectEmulation()
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

void Emulation::disconnectComponent(string id, xmlNodePtr children)
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

void Emulation::destroyEmulation()
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

void Emulation::destroyComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
		return;
	
	delete component;
	
	setComponent(id, NULL);
}

bool Emulation::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case EMULATION_UPDATE:
			if (didUpdate)
				didUpdate(userData);
			
			return true;
			
		case EMULATION_RUN_ALERT:
			if (data && runAlert)
			{
				if (runAlert)
					runAlert(userData, *((string *)data));
				
				return true;
			}
			break;
			
		case EMULATION_ASSERT_ACTIVE:
			activeCount++;
			return true;
			
		case EMULATION_CLEAR_ACTIVE:
			if (activeCount <= 0)
				return false;
			
			activeCount--;
			return true;
			
		case EMULATION_SET_STATE:
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->state = *((string *)data);
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
			
		case EMULATION_SET_IMAGE:
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->image = *((string *)data);
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
			
		case EMULATION_SET_HOT_PLUGGABLE:
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					deviceInfo->hotPluggable = *((bool *)data);
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
			
		case EMULATION_CREATE_CANVAS:
			if (data && createCanvas)
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					string title = deviceInfo->label;
					
					*ref = createCanvas(userData, title);
					
					deviceInfo->canvases.push_back(*ref);
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
			
		case EMULATION_DESTROY_CANVAS:
			if (data && destroyCanvas)
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent **ref = (OEComponent **)data;
					
					OEComponents &canvases = deviceInfo->canvases;
					OEComponents::iterator first = canvases.begin();
					OEComponents::iterator last = canvases.end();
					OEComponents::iterator i = remove(first, last, *ref);
					if (i != last)
						canvases.erase(i, last);
					
					destroyCanvas(userData, *ref);
					
					*ref = NULL;
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
			
		case EMULATION_SET_STORAGE:
			{
				EmulationDeviceInfo *deviceInfo = getDeviceInfo(getDeviceId(sender));
				if (deviceInfo)
				{
					OEComponent *ref = (OEComponent *)data;
					
					deviceInfo->storage = ref;
					
					return postMessage(sender, EMULATION_UPDATE, NULL);
				}
			}
			break;
	}
	
	return OEComponent::postMessage(sender, message, data);
}

bool Emulation::hasValueProperty(string value, string propertyName)
{
	return (value.find("${" + propertyName + "}") != string::npos);
}

string Emulation::parseValueProperties(string value, map<string, string> &propertiesMap)
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

void Emulation::setDeviceId(string &id, string deviceId)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		id = deviceId;
	else
		id = deviceId + "." + id.substr(0, dotIndex + 1);
}

string Emulation::getDeviceId(string id)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		return id;
	
	return id.substr(0, dotIndex);
}

string Emulation::getDeviceId(OEComponent *component)
{
	for (map<string, OEComponent *>::iterator i = componentsMap.begin();
		 i != componentsMap.end();
		 i++)
	{
		if (i->second == component)
			return getDeviceId(i->first);
	}
	
	return "";
}

/*
bool Emulation::isDevice(string deviceId)
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

OEIdList Emulation::getDeviceIds()
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
