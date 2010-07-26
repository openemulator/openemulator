
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

#include "OEAddress.h"

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
			OELog("could not open \"" + path + "\"");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			is_open = package->readFile(OE_PACKAGE_DML_FILENAME, &data);
			
			if (!is_open)
				OELog(string("could not read ") + OE_PACKAGE_DML_FILENAME +
					  " in \"" + path + "\"");
		}
		else
			OELog("could not open package \"" + path + "\"");
	}
	else
		OELog("could not identify type of \"" + path + "\"");
	
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
			OELog("could not parse DML of \"" + path + "\"");
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
				OELog("could not open \"" + path + "\"");
		}
		else
			OELog("could not dump DML for \"" + path + "\"");
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
					OELog(string("could not write ") + OE_PACKAGE_DML_FILENAME +
						  " in \"" + path + "\"");
			}
			else
				OELog("could not dump DML for \"" + path + "\"");
		}
		else
			OELog("could not open package \"" + path + "\"");
	}
	else
		OELog("could not identify type of \"" + path + "\"");
	
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
	OEDML emulation(path);
}

bool OEDML::remove(string ref)
{
	OEAddress address;
	address.setDevice(OEAddress(ref).getDevice());
	
	xmlNodePtr node = getNode(address.ref());
	if (!node)
	{
		OELog("could not find node for \"" + ref + "\"");
		return false;
	}
	
	for (xmlNodePtr childNode = node->children;
		 childNode;
		 childNode++)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			continue;
		
		string inletRef = address.ref(getXMLProperty(childNode, "ref"));
		string outletRef = followRef(inletRef);
		if (outletRef == "")
			continue;
		
		if (!remove(outletRef))
			return false;
	}
	
	if (node->next && (node->next->type == XML_TEXT_NODE))
		xmlUnlinkNode(node->next);
	xmlUnlinkNode(node);
	xmlFreeNode(node);
	
	return true;
}

//
// DML Operations
//
bool OEDML::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(rootNode, "version") == "1.0");
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
// DML Functions
//
xmlNodePtr OEDML::getNode(string ref)
{
	if (!doc)
		return NULL;
	
	OEAddress address(ref);
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr deviceNode = rootNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		if (getXMLProperty(deviceNode, "name") != address.getDevice())
			continue;
		
		if (address.getComponent() == "")
			return deviceNode;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getXMLProperty(componentNode, "name") != address.getComponent())
				continue;
			
			if (address.getProperty() == "")
				return componentNode;
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getXMLProperty(connectionNode, "name") != address.getProperty())
					continue;
				
				return connectionNode;
			}
		}
	}
	
	return NULL;
}

string OEDML::followRef(string ref)
{
	OEAddress address(ref);
	
	xmlNodePtr connectionNode = getNode(ref);
	if (!connectionNode)
		return "";
	
	string connectionRef = getXMLProperty(connectionNode, "ref");
	if (connectionRef == "")
		return "";
	
	return address.ref(connectionRef);
}

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

string OEDML::getXMLProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEDML::setXMLProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}
