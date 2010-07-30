
/**
 * libemulation
 * OEDML
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Encapsulates a DML document.
 */

#include <sstream>
#include <fstream>

#include "OEDML.h"

OEDML::OEDML()
{
	init();
}

OEDML::OEDML(string path)
{
	init();
	open(path);
}

OEDML::~OEDML()
{
	close();
	
	if (doc)
		xmlFreeDoc(doc);
}

void OEDML::init()
{
	is_open = false;
	
	package = NULL;
	doc = NULL;
}

bool OEDML::open(string path)
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
			is_open = package->readFile(OE_PACKAGE_DML_FILENAME, &data);
			
			if (!is_open)
				OELog("could not read '" OE_PACKAGE_DML_FILENAME
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
							OE_PACKAGE_DML_FILENAME,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			OELog("could not parse DML of '" + path + "'");
		}
	}
	
	if (is_open)
	{
		is_open = validate();
		if (!is_open)
			OELog("unknown DML version");
	}
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEDML::isOpen()
{
	return is_open;
}

bool OEDML::save(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		update();
		
		if (dump(&data))
		{
			is_open = writeFile(path, &data);
			
			if (!is_open)
				OELog("could not open '" + path + "'");
		}
		else
			OELog("could not dump DML for '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			update();
			
			if (dump(&data))
			{
				is_open = package->writeFile(OE_PACKAGE_DML_FILENAME, &data);
				if (!is_open)
					OELog("could not write " OE_PACKAGE_DML_FILENAME
						  " in '" + path + "'");
			}
			else
				OELog("could not dump DML for '" + path + "'");
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

void OEDML::close()
{
	is_open = false;
	
	if (package)
		delete package;
	package = NULL;
}

bool OEDML::add(string path, OEConnections &connections)
{
	OEDML dml(path);
	
//	OEConnections nameMap = buildNameMap(path);
//	if (!rename(dml, nameMap))
//		return false;
	
//	xmlDocPtr insertionNode = getInsertPoint(dml);
	
//	return insert(insertionNode, dml);
	
	return true;
}

bool OEDML::removeDevice(string deviceName)
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
		
		xmlNodePtr connectionNode = getConnectionNode(deviceNode,
													  getRef(node));
		if (!connectionNode)
			return false;
		
		if (!removeDevice(getDeviceName(getRef(connectionNode))))
			return false;
	}
	
	removeDevice(deviceNode);
	
	return true;
}

void OEDML::removeDevice(xmlNodePtr deviceNode)
{
	if (deviceNode->next && (deviceNode->next->type == XML_TEXT_NODE))
		xmlUnlinkNode(deviceNode->next);
	
	xmlUnlinkNode(deviceNode);
	xmlFreeNode(deviceNode);
}

//
// Operations
//
bool OEDML::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
}

bool OEDML::dump(OEData *data)
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

void OEDML::update()
{
}

//
// DML
//
xmlNodePtr OEDML::getDeviceNode(string deviceName)
{
	if ((deviceName != "") && doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		xmlNodePtr deviceNode = getChildNode(rootNode, "device", deviceName);
		
		if (deviceNode)
			return deviceNode;
	}
	
	OELog("could not find device '" + deviceName + "'");
	
	return NULL;
}

xmlNodePtr OEDML::getConnectionNode(xmlNodePtr deviceNode, string ref)
{
	string componentName = getComponentName(ref);
	xmlNodePtr componentNode = getChildNode(deviceNode,
											"component", componentName);
	if (componentNode)
	{
		string connectionName = getConnectionName(ref);
		xmlNodePtr connectionNode = getChildNode(componentNode,
												 "connection", connectionName);
		if (connectionNode)
			return connectionNode;
	}
	
	OELog("inlet ref '" + ref + "' does not exist");
	
	return NULL;
}

xmlNodePtr OEDML::getChildNode(xmlNodePtr node, string tag, string name)
{
	if (!node)
		return NULL;
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (string((char *) (childNode->name)) == tag)
			continue;
		
		if (getName(childNode) != name)
			continue;
		
		return node;
	}
	
	return NULL;
}

string OEDML::getName(xmlNodePtr node)
{
	string name = getNodeProperty(node, "name");
	
	return filterName(name);
}

string OEDML::filterName(string name)
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

string OEDML::getRef(xmlNodePtr node)
{
	string ref = getNodeProperty(node, "ref");
	
	return filterRef(ref);
}

string OEDML::filterRef(string name)
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


string OEDML::getDeviceName(string ref)
{
	int index = ref.find(OE_DEVICE_SEPARATOR);
	if (index == string::npos)
		return "";
	
	return ref.substr(0, index);
}

string OEDML::getComponentName(string ref)
{
	int index;
	
	index = ref.find(OE_DEVICE_SEPARATOR);
	if (index != string::npos)
		ref = ref.substr(index + sizeof(OE_DEVICE_SEPARATOR) - 1);
	
	index = ref.find(OE_CONNECTION_SEPARATOR);
	if (index != string::npos)
		ref = ref.substr(0, index);
	
	return ref.substr(0, index);
}

string OEDML::getConnectionName(string ref)
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
		OERef outletRef = getOutletForInlet(documentDML, inletRef);
		return getNodeForRef(documentDML, outletRef);
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
string OEDML::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEDML::getPathExtension(string path)
{
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	return path.substr(extensionIndex + 1);
}

bool OEDML::readFile(string path, OEData *data)
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

bool OEDML::writeFile(string path, OEData *data)
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

string OEDML::getNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEDML::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}
