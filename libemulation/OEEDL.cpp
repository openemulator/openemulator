
/**
 * libemulation
 * OEEDL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

#include <sstream>
#include <fstream>

#include "OEEDL.h"

OEEDL::OEEDL()
{
	init();
}

OEEDL::~OEEDL()
{
	close();
	
	if (doc)
		xmlFreeDoc(doc);
}

void OEEDL::init()
{
	is_open = false;
	
	package = NULL;
	doc = NULL;
}

bool OEEDL::open(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_FILE_PATH_EXTENSION)
	{
		is_open = readFile(path, &data);
		
		if (!is_open)
			edlLog("could not open '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			is_open = package->readFile(OE_PACKAGE_EDL_PATH, &data);
			
			if (!is_open)
				edlLog("could not read '" OE_PACKAGE_EDL_PATH
					  "' in '" + path + "'");
		}
		else
			edlLog("could not open package '" + path + "'");
	}
	else
		edlLog("could not identify type of '" + path + "'");
	
	if (is_open)
	{
		doc = xmlReadMemory(&data[0],
							data.size(),
							OE_PACKAGE_EDL_PATH,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			edlLog("could not parse EDL in '" + path + "'");
		}
	}
	
	if (is_open)
	{
		is_open = validate();
		if (!is_open)
			edlLog("unknown EDL version");
	}
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEDL::isOpen()
{
	return is_open;
}

bool OEEDL::save(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_FILE_PATH_EXTENSION)
	{
		if (updateEmulation())
		{
			if (dump(&data))
			{
				is_open = writeFile(path, &data);
				
				if (!is_open)
					edlLog("could not open '" + path + "'");
			}
			else
				edlLog("could not dump EDL for '" + path + "'");
		}
		else
			edlLog("could not update the configuration for '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage(path);
		
		if (package && package->isOpen())
		{
			if (updateEmulation())
			{
				if (dump(&data))
				{
					is_open = package->writeFile(OE_PACKAGE_EDL_PATH, &data);
					if (!is_open)
						edlLog("could not write '" OE_PACKAGE_EDL_PATH
							  "' in '" + path + "'");
				}
				else
					edlLog("could not dump EDL for '" + path + "'");
			}
			
			delete package;
			package = NULL;
		}
		else
			edlLog("could not open '" + path + "'");
	}
	else
		edlLog("could not identify type of '" + path + "'");
	
	if (!is_open)
		close();
	
	return is_open;
}

void OEEDL::close()
{
	is_open = false;
	
	if (package)
		delete package;
	package = NULL;
}

OEHeaderInfo OEEDL::getHeaderInfo()
{
	OEHeaderInfo headerInfo;
	
	if (doc)
	{
		xmlNodePtr node = xmlDocGetRootElement(doc);
		headerInfo.label = getNodeProperty(node, "label");
		headerInfo.image = getNodeProperty(node, "image");
		headerInfo.description = getNodeProperty(node, "description");
	}
	
	return headerInfo;
}

void OEEDL::setWindowFrame(string windowFrame)
{
	if (doc)
	{
		xmlNodePtr node = xmlDocGetRootElement(doc);
		setNodeProperty(node, "windowFrame", windowFrame);
	}
}

string OEEDL::getWindowFrame()
{
	string windowFrame;
	
	if (doc)
	{
		xmlNodePtr node = xmlDocGetRootElement(doc);
		windowFrame = getNodeProperty(node, "windowFrame");
	}
	
	return windowFrame;
}

OEPortsInfo OEEDL::getFreePortsInfo()
{
	OEPortsInfo freePortsInfo;
	
	if (doc)
	{
		xmlNodePtr node = xmlDocGetRootElement(doc);
	}
	
	return freePortsInfo;
}

OEConnectorsInfo OEEDL::getFreeConnectorsInfo()
{
	OEConnectorsInfo connectorsInfo;
	
	if (doc)
	{
		xmlNodePtr node = xmlDocGetRootElement(doc);
	}
	
	return connectorsInfo;
}

bool OEEDL::addEDL(string path, OEIdMap connectionMap)
{
	if (!doc)
		return false;
	
	// Load new EDL
	OEEDL edl;
	edl.open(path);
	if (!edl.isOpen())
		return false;
	
	// Build name map for new EDL
	OEIdList deviceIds = getDeviceIds();
	OEIdList newDeviceIds = edl.getDeviceIds();
	OEIdMap nameMap = buildNameMap(deviceIds, newDeviceIds);
	
	// Rename EDL id's, and rename connection map
	edl.rename(nameMap);
	renameConnectionMap(connectionMap, nameMap);
	
	// Insert EDL
//	xmlNodePtr insertionNode = findInsertionPoint(connectionMap.begin()->first)
//	insert(insertionNode, &edl);

	// Connect port inlets
	connect();
	
	return true;
}

bool OEEDL::removeDevice(string deviceId)
{
	// Verify device exists
	if (!isDevice(deviceId))
	{
		edlLog("could not find '" + deviceId + "'");
		return false;
	}
	
	// Recursively remove devices connected to this device
	if (!removeConnectedDevices(deviceId))
		return false;
	
	// Remove references to this device
	disconnect(deviceId);
	
	// Remove elements matching this device
	removeElements(deviceId);
	
	return true;
}


bool OEEDL::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
}

bool OEEDL::dump(OEData *data)
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

bool OEEDL::updateEmulation()
{
	return true;
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
	/* Idea
	 - Buscar el punto de inserción (buscar el device del primer port,
	 y seleccionar el punto antes del proximo device
	 - Insertar los elementos que matcheen con algun device conectado
	 - Iterar sobre los ports, luego sobre los inlets, y definir referencias
	 - Iterar sobre los conectores, luego sobre los inlets, y definir referencias
	 */
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

bool OEEDL::isDevice(string deviceId)
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

OEIdList OEEDL::getDeviceIds()
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

void OEEDL::setDeviceId(string &id, string deviceId)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		id = deviceId;
	else
		id = deviceId + "." + id.substr(0, dotIndex + 1);
}

string OEEDL::getDeviceId(string id)
{
	int dotIndex = id.find_first_of('.');
	
	if (dotIndex == string::npos)
		return id;
	
	return id.substr(0, dotIndex - 1);
}

string OEEDL::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}

bool OEEDL::hasNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	return (value != NULL);
}

string OEEDL::getNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

string OEEDL::getPathExtension(string path)
{
	if (path.rfind(OE_PATH_SEPARATOR) == (path.length() - 1))
		path = path.substr(0, path.length() - 1);
	
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	else
		return path.substr(extensionIndex + 1);
}

bool OEEDL::writeFile(string path, OEData *data)
{
	bool success = false;
	
	ofstream file(path.c_str());
	
	if (file.is_open())
	{
		file.write(&data->front(), data->size());
		success = file.good();
		file.close();
	}
	
	return success;
}

bool OEEDL::readFile(string path, OEData *data)
{
	bool success = false;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data->resize(size);
		file.read(&data->front(), size);
		success = file.good();
		file.close();
	}
	
	return success;
}

void OEEDL::edlLog(string message)
{
	cerr << "libemulation: " << message << endl;
}




/**
 * libemulation
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses an EDL file.
 */

/*OEInfo::~OEInfo()
 {
 removeDevices();
 removePorts(inlets);
 removePorts(outlets);
 }
 
 bool OEInfo::open(string path)
 {
 if (!OEEDL::open(path))
 return false;
 
 if (analyze())
 return true;
 
 close();
 
 edlLog("in '" + path + "'");
 
 return false;
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
 
 string OEInfo::getLabel()
 {
 return label;
 }
 
 string OEInfo::getImage()
 {
 return image;
 }
 
 string OEInfo::getDescription()
 {
 return description;
 }
 
 string OEInfo::getType()
 {
 return type;
 }
 
 OEDevices *OEInfo::getDevices()
 {
 return &devices;
 }
 
 OEPorts *OEInfo::getInlets()
 {
 return &inlets;
 }
 
 OEPorts *OEInfo::getOutlets()
 {
 return &outlets;
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
 /*	
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
