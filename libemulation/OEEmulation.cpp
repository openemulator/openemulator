
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

OEEmulation::OEEmulation() : OEEDL()
{
	alertCallback = NULL;
	addCanvasCallback = NULL;
	removeCanvasCallback = NULL;
	
	setComponent("emulation", this);
}

OEEmulation::~OEEmulation()
{
	close();
}

void OEEmulation::setResourcePath(string path)
{
	resourcePath = path;
}

void OEEmulation::setAlertCallback(OERunAlertCallback alertCallback)
{
	this->alertCallback = alertCallback;
}

void OEEmulation::setAddCanvasCallback(OEAddCanvasCallback addCanvasCallback,
									   void *userData)
{
	this->addCanvasCallback = addCanvasCallback;
	addCanvasCallbackUserData = userData;
}

void OEEmulation::setRemoveCanvasCallback(OERemoveCanvasCallback
										  removeCanvasCallback,
										  void *userData)
{
	this->addCanvasCallback = addCanvasCallback;
	addCanvasCallbackUserData = userData;
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
		package = new OEPackage(path);
		
		if (package && package->isOpen())
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

void OEEmulation::close()
{
	disconnectEmulation();
	destroyEmulation();
	
	OEEDL::close();
}

OEDevicesInfo OEEmulation::getDevicesInfo()
{
	OEDevicesInfo devicesInfo;
	
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	}
	
	return devicesInfo;
}

bool OEEmulation::setComponent(string id, OEComponent *component)
{
	if (component)
		components[id] = component;
	else
		components.erase(id);
	
	return true;
}

OEComponent *OEEmulation::getComponent(string id)
{
	if (!components.count(id))
		return NULL;
	
	return components[id];
}

bool OEEmulation::addEDL(string path, OEIdMap connectionMap)
{
	if (!doc)
		return false;
	
	// Load new EDL
	OEEDL edl;
	edl.open(path);
	if (!edl.isOpen())
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

bool OEEmulation::dumpEmulation(OEData *data)
{
	xmlChar *p;
	int size;
	
	xmlDocDumpMemory(doc, &p, &size);
	if (!p)
		return false;
	
	data->resize(size);
	memcpy(&data->front(), p, size);
	
	return true;
}

bool OEEmulation::createEmulation()
{
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
				
				OEData *data;
				string parsedSrc = parseValueProperties(src, propertiesMap);
				
				if (component->getData(name, &data))
				{
					if (data)
					{
						if (!package->writeFile(parsedSrc, data))
							log("could not write '" + src + "'");
					}
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

bool OEEmulation::mount(string path)
{
	return delegate(this, EMULATION_MOUNT, &path);
}

bool OEEmulation::validate(string path)
{
	return delegate(this, EMULATION_VALIDATE, &path);
}

bool OEEmulation::postMessage(OEComponent *sender, int message, void *data)
{
	if (message == EMULATION_SET_DEVICEINFO)
	{
		OEEmulationSetDeviceInfo *setDeviceInfo;
		setDeviceInfo = (OEEmulationSetDeviceInfo *)data;
		
		string deviceId = setDeviceInfo->deviceId;
		devicesInfo[deviceId] = setDeviceInfo->deviceInfo;
	}
	else if (message == EMULATION_ADD_CANVAS)
	{
		OEComponent **ref = (OEComponent **)data;
		*ref = addCanvasCallback(addCanvasCallbackUserData);
	}
	else if (message == EMULATION_REMOVE_CANVAS)
	{
		OEComponent **ref = (OEComponent **)data;
		removeCanvasCallback(*ref, removeCanvasCallbackUserData);
		*ref = NULL;
	}
	else if (message == EMULATION_RUN_ALERT)
	{
		if (alertCallback)
		{
			string *message = (string *)data;
			alertCallback(*message);
		}
		else
			return false;
	}
	else
		return false;
	
	return true;
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
	
	return id.substr(0, dotIndex - 1);
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

//
// Analysis
//
bool OEInfo::analyze()
{
	removeDevices();
	removePorts(inlets);
	removePorts(outlets);
	
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	analyzeHeader(node);
	analyzeDevices(node);
	return analyzeConnections();
}

void OEInfo::analyzeHeader(xmlNodePtr node)
{
}

void OEInfo::analyzeDevices(xmlNodePtr node)
{
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			addDevice(childNode);
}

void OEInfo::addDevice(xmlNodePtr node)
{
	OEDevice *device = new OEDevice();
	if (!device)
		return;
	
	device->name = getNodeProperty(node, "name");
	device->type = getNodeProperty(node, "type");
	device->label = getNodeProperty(node, "label");
	device->image = getNodeProperty(node, "image");
	
	device->connectionLabel = "";
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			addPort(childNode, device, inlets);
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			addPort(childNode, device, outlets);
		else if (!xmlStrcmp(childNode->name, BAD_CAST "setting"))
			addSetting(childNode, device);
	}
	
	devices.push_back(device);
}

void OEInfo::addSetting(xmlNodePtr node, OEDevice *device)
{
	OESetting *setting = new OESetting();
	if (!setting)
		return;
	
	setting->ref = getNodeProperty(node, "ref");
	setting->type = getNodeProperty(node, "type");
	setting->options = getNodeProperty(node, "options");
	setting->label = getNodeProperty(node, "label");
	
	device->settings.push_back(setting);
}

void OEInfo::addPort(xmlNodePtr node, OEDevice *device, OEPorts &ports)
{
	OEPort *port = new OEPort();
	if (!port)
		return;
	
	port->ref = getNodeProperty(node, "ref");
	port->type = getNodeProperty(node, "type");
	port->label = getNodeProperty(node, "label");
	port->image = getNodeProperty(node, "image");
	
	if (port->label == "")
		port->label = device->label;
	if (port->image == "")
		port->image = device->image;
	
	port->connection = NULL;
	port->device = device;
	
	ports.push_back(port);
}

void OEInfo::removeDevices()
{
	for (OEDevices::iterator device = devices.begin();
		 device != devices.end();
		 device++)
	{
		OESettings *settings = &(*device)->settings;
		for (OESettings::iterator setting = settings->begin();
			 setting != settings->end();
			 setting++)
			delete *setting;
		
		delete *device;
	}
	
	devices.clear();
}

void OEInfo::removePorts(OEPorts &ports)
{
	for (OEPorts::iterator port = ports.begin();
		 port != ports.end();
		 port++)
		delete *port;
	
	ports.clear();
}

bool OEInfo::analyzeConnections()
{
	// Check inlets
	for (OEPorts::iterator inlet = inlets.begin();
		 inlet != inlets.end();
		 inlet++)
	{
		// Follow inlet connection
		string deviceName = (*inlet)->device->name;
		xmlNodePtr deviceNode = getDeviceNode(deviceName);
		if (!deviceNode)
			return false;
		
		xmlNodePtr connectionNode = getConnectionNode(deviceNode, (*inlet)->ref);
		if (!connectionNode)
			return false;
		
		string connectionRef = getNodeRef(connectionNode, deviceName);
		if (connectionRef == "")
			continue;
		
		// Search outlet
		for (OEPorts::iterator outlet = outlets.begin();
			 outlet != outlets.end();
			 outlet++)
		{
			if ((*outlet)->ref == connectionRef)
			{
				if ((*outlet)->connection)
				{
					edlLog("reconnection of inlet '" + (*inlet)->ref + "'");
					
					return false;
				}
				
				(*inlet)->connection = *outlet;
				(*outlet)->connection = *inlet;
			}
		}
		
		if (!(*inlet)->connection)
		{
			edlLog("could not find '" + (*inlet)->ref + "'");
			
			return false;
		}
	}
	
	// Analyze connection labels
	for (OEDevices::iterator device = devices.begin();
		 device != devices.end();
		 device++)
		(*device)->connectionLabel = buildConnectionLabel((*device)->name);
	
	return true;
}

string OEInfo::buildConnectionLabel(string deviceName)
{
	string label;
	
	// Find all outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		if (getDeviceName((*outlet)->ref) == deviceName)
		{
			vector<string> visitedRefs;
			
			if (label != "")
				label += ", ";
			label += buildConnectionLabel(*outlet, visitedRefs);
		}
	}
	
	return label;
}

string OEInfo::buildConnectionLabel(OEPort *outlet, vector<string> &visitedRefs)
{
	string deviceLabel = outlet->device->label;
	
	// Get the connected inlet for this outlet
	OEPort *inlet = outlet->connection;
	if (!inlet)
		return deviceLabel;
	
	string deviceName = getDeviceName(inlet->ref);
	
	// Find first outlet of the inlet's device
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		if (getDeviceName((*outlet)->ref) == deviceName)
		{
			if (findCircularConnection((*outlet)->ref, visitedRefs))
				return deviceLabel;
			
			return buildConnectionLabel(*outlet, visitedRefs) + " " + inlet->label;
		}
	}
	
	// The device has no outlets
	return deviceLabel;
}

bool OEInfo::findCircularConnection(string ref, vector<string> &visitedRefs)
{
	for (vector<string>::iterator visitedRef = visitedRefs.begin();
		 visitedRef != visitedRefs.end();
		 visitedRef++)
	{
		if (ref == *visitedRef)
			return true;
	}
	
	visitedRefs.push_back(ref);
	
	return false;
}
*/
