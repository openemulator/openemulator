
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

OEEDL::OEEDL(const string &path)
{
	init();
	
	open(path);
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

bool OEEDL::open(const string &path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		is_open = readFile(path, &data);
		
		if (!is_open)
			OELog("could not open '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			is_open = package->readFile(OE_PACKAGE_EDL_FILENAME, &data);
			
			if (!is_open)
				OELog("could not read '" OE_PACKAGE_EDL_FILENAME
					  "' in '" + path + "'");
		}
		else
			OELog("could not open package '" + path + "'");
	}
	else
		OELog("could not identify type of '" + path + "'");
	
	if (is_open)
	{
		doc = xmlReadMemory(&data[0],
							data.size(),
							OE_PACKAGE_EDL_FILENAME,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			OELog("could not parse EDL, path '" + path + "'");
		}
	}
	
	if (is_open)
	{
		is_open = validate();
		if (!is_open)
			OELog("unknown EDL version");
	}
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEDL::isOpen()
{
	return is_open;
}

bool OEEDL::save(const string &path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		if (update())
		{
			if (dump(&data))
			{
				is_open = writeFile(path, &data);
				
				if (!is_open)
					OELog("could not open '" + path + "'");
			}
			else
				OELog("could not dump EDL, path '" + path + "'");
		}
		else
			OELog("could not update '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		
		if (package && package->isOpen())
		{
			if (update())
			{
				if (dump(&data))
				{
					is_open = package->writeFile(OE_PACKAGE_EDL_FILENAME, &data);
					if (!is_open)
						OELog("could not write " OE_PACKAGE_EDL_FILENAME
							  " in '" + path + "'");
				}
				else
					OELog("could not dump EDL, path '" + path + "'");
			}
			
			delete package;
			
			package = NULL;
		}
		else
			OELog("could not open '" + path + "'");
	}
	else
		OELog("could not identify type of '" + path + "'");
	
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

bool OEEDL::addEDL(const string &path, OEConnections &connections)
{
	OEEDL edl(path);
	
	//	OEConnections nameMap = buildNameMap(path);
	//	if (!rename(edl, nameMap))
	//		return false;
	
	//	xmlDocPtr insertionNode = getInsertPoint(edl);
	
	//	return insert(insertionNode, edl);
	
	return true;
}

bool OEEDL::removeDevice(const string &deviceName)
{
	xmlNodePtr deviceNode = getDeviceNode(deviceName);
	if (!deviceNode)
		return false;
	
	for (xmlNodePtr node = deviceNode->children;
		 node;
		 node++)
	{
		if (xmlStrcmp(node->name, BAD_CAST "inlet"))
			continue;
		
		string connectionRef = getNodeRef(node, deviceName);
		
		xmlNodePtr connectionNode = getConnectionNode(deviceNode, connectionRef);
		if (!connectionNode)
			return false;
		
		string componentRef = getNodeRef(connectionNode, deviceName);
		
		if (!removeDevice(getDeviceName(componentRef)))
			return false;
	}
	
	removeDevice(deviceNode);
	
	return true;
}

void OEEDL::removeDevice(xmlNodePtr deviceNode)
{
	if (deviceNode->next && (deviceNode->next->type == XML_TEXT_NODE))
		xmlUnlinkNode(deviceNode->next);
	
	xmlUnlinkNode(deviceNode);
	xmlFreeNode(deviceNode);
}

//
// Operations
//
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

bool OEEDL::update()
{
	return true;
}

//
// EDL
//
xmlNodePtr OEEDL::getDeviceNode(string deviceName)
{
	if ((deviceName != "") && doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		xmlNodePtr deviceNode = getChildNodeWithName(rootNode, "device", deviceName);
		
		if (deviceNode)
			return deviceNode;
	}
	
	OELog("could not find device '" + deviceName + "'");
	
	return NULL;
}

xmlNodePtr OEEDL::getConnectionNode(xmlNodePtr deviceNode, string ref)
{
	string componentName = getComponentName(ref);
	xmlNodePtr componentNode = getChildNodeWithName(deviceNode,
													"component",
													componentName);
	if (componentNode)
	{
		string connectionName = getConnectionName(ref);
		xmlNodePtr connectionNode = getChildNodeWithName(componentNode,
														 "connection",
														 connectionName);
		if (connectionNode)
			return connectionNode;
	}
	
	OELog("inlet ref '" + ref + "' does not exist");
	
	return NULL;
}

xmlNodePtr OEEDL::getChildNodeWithName(xmlNodePtr node, string elementName, string name)
{
	if (!node)
		return NULL;
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (childNode->type != XML_ELEMENT_NODE)
			continue;
		
		if (!xmlStrcmp(childNode->name, BAD_CAST elementName.c_str()))
			if (getNodeName(childNode) == name)
				return childNode;
	}
	
	return NULL;
}

string OEEDL::getNodeName(xmlNodePtr node)
{
	string name = getNodeProperty(node, "name");
	
	return filterName(name);
}

string OEEDL::filterName(string name)
{
	string out;
	
	for (string::iterator i = name.begin();
		 i != name.end();
		 i++)
	{
		if (isalpha(*i) || isnumber(*i))
			out += *i;
	}
	
	return out;
}

string OEEDL::getNodeRef(xmlNodePtr node, string deviceName)
{
	string ref = getNodeProperty(node, "ref");
	
	ref = filterRef(ref);
	
	if ((ref != "") && (getDeviceName(ref) == ""))
		ref = deviceName + OE_DEVICE_SEPARATOR + ref;
	
	return ref;
}

string OEEDL::filterRef(string name)
{
	string out;
	
	for (string::iterator i = name.begin();
		 i != name.end();
		 i++)
	{
		if (isalpha(*i) || isnumber(*i) ||
			(*i == OE_DEVICE_SEPARATOR[0]) ||
			(*i == OE_CONNECTION_SEPARATOR[0]))
			out += *i;
	}
	
	return out;
}

string OEEDL::getDeviceName(string ref)
{
	int index = ref.find(OE_DEVICE_SEPARATOR);
	if (index == string::npos)
		return "";
	
	return ref.substr(0, index);
}

string OEEDL::getComponentName(string ref)
{
	int index;
	
	index = ref.find(OE_DEVICE_SEPARATOR);
	if (index != string::npos)
		ref = ref.substr(index + sizeof(OE_DEVICE_SEPARATOR) - 1);
	
	index = ref.find(OE_CONNECTION_SEPARATOR);
	if (index != string::npos)
		ref = ref.substr(0, index);
	
	return ref;
}

string OEEDL::getConnectionName(string ref)
{
	int index = ref.find(OE_CONNECTION_SEPARATOR);
	if (index == string::npos)
		return "";
	
	return ref.substr(index + sizeof(OE_CONNECTION_SEPARATOR) - 1);
}

/*
 xmlNodePtr OEEmulation::getNodeOfFirstInlet(xmlDocPtr doc, OERef ref)
 {
 OERef deviceRef = ref.getDeviceRef();
 xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
 if (!deviceNode)
 return NULL;
 
 for(xmlNodePtr inletNode = deviceNode->children;
 inletNode;
 inletNode = inletNode->next)
 {
 if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
 continue;
 
 OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
 OERef outletRef = getOutletForInlet(documentEDL, inletRef);
 return getNodeForRef(documentEDL, outletRef);
 }
 
 return NULL;
 }
 
 xmlNodePtr OEEmulation::getNodeOfLastInlet(xmlDocPtr doc,
 OERef ref, vector<OERef> &visitedRefs)
 {
 OERef deviceRef = ref.getDeviceRef();
 xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
 if (!deviceNode)
 return NULL;
 
 // Avoid circular reference
 for (vector<OERef>::iterator v = visitedRefs.begin();
 v != visitedRefs.end();
 v++)
 {
 if (deviceRef == *v)
 return NULL;
 }
 visitedRefs.push_back(deviceRef);
 
 OERef lastRef;
 for(xmlNodePtr inletNode = deviceNode->children;
 inletNode;
 inletNode = inletNode->next)
 {
 if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
 continue;
 
 OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
 OERef outletRef = getOutletForInlet(doc, inletRef);
 if (!outletRef.isEmpty())
 lastRef = outletRef;
 }
 
 if (lastRef.isEmpty())
 return deviceNode;
 
 return getNodeOfLastInlet(doc, lastRef, visitedRefs);
 }
 
 xmlNodePtr OEEmulation::getNodeOfPreviousInlet(xmlDocPtr doc, OERef ref)
 {
 OERef deviceRef = ref.getDeviceRef();
 xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
 if (!deviceNode)
 return xmlDocGetRootElement(doc);
 
 OERef prevRef;
 for(xmlNodePtr inletNode = deviceNode->children;
 inletNode;
 inletNode = inletNode->next)
 {
 if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
 continue;
 
 OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
 if (inletRef == ref)
 {
 if (prevRef.isEmpty())
 return deviceNode;
 else
 {
 vector<OERef> visitedRefs;
 return getNodeOfLastInlet(doc, prevRef, visitedRefs);
 }
 }
 
 OERef outletRef = getOutletForInlet(doc, inletRef);
 if (!outletRef.isEmpty())
 prevRef = outletRef;
 }
 
 return deviceNode;
 }
 */














//
// Helpers
//
string OEEDL::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEEDL::getPathExtension(string path)
{
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	return path.substr(extensionIndex + 1);
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

string OEEDL::getNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}
