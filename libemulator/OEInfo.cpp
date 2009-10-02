
/**
 * libemulator
 * Parses emulation info
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <fstream>

#include <libxml/parser.h>

#include "OEInfo.h"
#include "Package.h"

OEInfo::OEInfo(string path)
{
	open = false;
	
	vector<char> data;
	
	string pathExtension = getPathExtension(path);
	if (pathExtension == "string")
	{
		if (!readFile(path, data))
			return;
	}
	else if (pathExtension == "emulation")
	{
		Package *package = new Package(path);
		if (package->isOpen())
		{
			if (!package->readFile(OE_DML_FILENAME, data))
				return;
		}
	}
	else
		return;
	
	xmlDocPtr dml;
	dml = xmlReadMemory(&data[0],
						data.size(),
						OE_DML_FILENAME,
						NULL,
						0);
	if (dml)
	{
		open = parse(dml);
	
		xmlFreeDoc(dml);
	}
}

OEInfo::OEInfo(xmlDocPtr dml)
{
	open = parse(dml);
}

OEInfo::~OEInfo()
{
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
		delete i->second;
	
	for (OEPorts::iterator i = outlets.begin();
		 i != outlets.end();
		 i++)
		delete i->second;
}

string OEInfo::getPathExtension(string path)
{
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	return path.substr(extensionIndex + 1);
}

bool OEInfo::readFile(string path, vector<char> &data)
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

string OEInfo::getNodeProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

string OEInfo::buildAbsoluteRef(string absoluteRef, string ref)
{
	if (ref.size() == 0)
		return string();
	
	if (ref.find("::") == string::npos)
	{
		int index = absoluteRef.find("::");
		if (index != string::npos)
			absoluteRef = absoluteRef.substr(index);
		
		ref = absoluteRef + "::" + ref;
	}
	
	return ref;
}

string OEInfo::getConnection(xmlDocPtr dml, string connectionRef)
{
	int deviceIndex = connectionRef.find(OE_DEVICE_SEP);
	if (deviceIndex == string::npos)
		return "";
	string deviceName = connectionRef.substr(deviceIndex);
	deviceIndex += sizeof(OE_DEVICE_SEP) - 1;
	
	int componentIndex = connectionRef.find(OE_COMPONENT_SEP, deviceIndex);
	if (componentIndex == string::npos)
		return "";
	string componentName = connectionRef.substr(deviceIndex,
											  componentIndex - deviceIndex);
	componentIndex += sizeof(OE_COMPONENT_SEP);
	
	string connectionName = connectionRef.substr(componentIndex);
	
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		if (getNodeProperty(deviceNode, "name") != deviceName)
			continue;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getNodeProperty(componentNode, "name") != componentName)
				continue;

			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getNodeProperty(connectionNode, "key") != connectionName)
					continue;

				return getNodeProperty(connectionNode, "ref");
			}
		}
	}
	
	return "";
}

bool OEInfo::parse(xmlDocPtr dml)
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	if (getNodeProperty(dmlNode, "version") != "1.0")
		return false;
	
	properties["label"] = getNodeProperty(dmlNode, "label");
	properties["image"] = getNodeProperty(dmlNode, "image");
	properties["description"] = getNodeProperty(dmlNode, "description");
	properties["group"] = getNodeProperty(dmlNode, "group");
	
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getNodeProperty(deviceNode, "name");
		
		for(xmlNodePtr node = deviceNode->children;
			node;
			node = node->next)
		{
			if (!xmlStrcmp(node->name, BAD_CAST "inlet"))
				parsePort(inlets, deviceName, node);
			else if (!xmlStrcmp(node->name, BAD_CAST "outlet"))
				parsePort(outlets, deviceName, node);
		}
	}
	
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
	{
		string inletRef = i->first;
		string outletRef = getConnection(dml, inletRef);
		if (!outletRef.size())
			continue;
		
		outletRef = buildAbsoluteRef(inletRef, outletRef);
		
		if (outlets.count(outletRef))
		{
			(*(inlets[inletRef]))["connected"] = "1";
			(*(outlets[outletRef]))["connected"] = "1";
		}
	}
	
	return true;
}

void OEInfo::parsePort(OEPorts &ports, string deviceName, xmlNodePtr node)
{
	string ref = buildAbsoluteRef(deviceName, getNodeProperty(node, "ref"));
	
	OEProperties *properties = ports[ref];
	(*properties)["type"] = getNodeProperty(node, "type");
	(*properties)["subtype"] = getNodeProperty(node, "subtype");
	(*properties)["label"] = getNodeProperty(node, "label");
	(*properties)["image"] = getNodeProperty(node, "image");
	(*properties)["connected"] = "0";
}

bool OEInfo::isOpen()
{
	return open;
}

OEProperties *OEInfo::getProperties()
{
	return &properties;
}

OEPorts *OEInfo::getInlets()
{
	return &inlets;
}

OEPorts *OEInfo::getOutlets()
{
	return &outlets;
}
