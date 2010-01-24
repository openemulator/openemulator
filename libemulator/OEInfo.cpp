
/**
 * libemulator
 * OEInfo
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <iostream>
#include <fstream>

#include <libxml/parser.h>

#include "OEInfo.h"
#include "OEPackage.h"

OEInfo::OEInfo(string path)
{
	loaded = false;
	
	vector<char> data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == "xml")
	{
		if (!readFile(path, data))
			return;
	}
	else if (pathExtension == "emulation")
	{
		OEPackage package(path);
		if (package.isOpen())
		{
			if (!package.readFile(OE_DML_FILENAME, data))
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
	if (!dml)
		return;
	
	parse(dml);
	xmlFreeDoc(dml);
}

OEInfo::OEInfo(xmlDocPtr dml)
{
	parse(dml);
}

OEInfo::OEInfo(const OEInfo &info)
{
	loaded = info.loaded;
	
	label = info.label;
	image = info.image;
	description = info.description;
	group = info.group;
	
	inlets = info.inlets;
	outlets = info.outlets;
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

string OEInfo::getXMLProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

bool OEInfo::validateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(node, "version") == "1.0");
}

OERef OEInfo::getOutletRefForInletRef(xmlDocPtr dml, OERef inletRef)
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		if (getXMLProperty(deviceNode, "name") != inletRef.getDevice())
			continue;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getXMLProperty(componentNode, "name") != inletRef.getComponent())
				continue;
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getXMLProperty(connectionNode, "key") != inletRef.getProperty())
					continue;
				
				string stringRef = getXMLProperty(connectionNode, "ref");
				if (stringRef.size())
					return inletRef.buildRef(stringRef);
			}
		}
	}
	
	return OERef();
}

OEPort *OEInfo::getOutlet(OERef outletRef)
{
	for (OEPorts::iterator o = outlets.begin();
		 o != outlets.end();
		 o++)
	{
		if (outletRef == o->ref)
			return &(*o);
	}
	
	return NULL;
}

string OEInfo::makeLabel(OEPort *outlet, vector<OERef> *refs)
{
	OEPort *inlet = outlet->connectedPort;
	if (!inlet)
		return "Unknown";
	
	for (OEPorts::iterator o = outlets.begin();
		 o != outlets.end();
		 o++)
	{
		if (inlet->ref == o->ref)
		{
			for (vector<OERef>::iterator r = refs->begin();
				 r != refs->end();
				 r++)
			{
				if (o->ref == *r)
					return inlet->label;
			}
			refs->push_back(o->ref);
			
			return makeLabel(&(*o), refs) + " " + inlet->label;
		}
	}
	
	return inlet->deviceLabel + " " + inlet->label;
}

OEPort OEInfo::parsePort(xmlNodePtr node,
						 OERef deviceRef,
						 string deviceLabel,
						 string deviceImage)
{
	OEPort port;
	
	port.ref = deviceRef.buildRef(getXMLProperty(node, "ref"));
	port.type = getXMLProperty(node, "type");
	port.category = getXMLProperty(node, "category");
	port.label = getXMLProperty(node, "label");
	port.image = getXMLProperty(node, "image");
	
	port.deviceLabel = deviceLabel;
	
	port.connectedPort = NULL;
	
	if (!port.label.size())
		port.label = deviceLabel;
	if (!port.image.size())
		port.image = deviceImage;
	
	// To-do: parse settings
	
	return port;
}

void OEInfo::parseDevice(xmlNodePtr node)
{
	OERef deviceRef = OERef(getXMLProperty(node, "name"));
	string deviceLabel = getXMLProperty(node, "label");
	string deviceImage = getXMLProperty(node, "image");
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			inlets.push_back(parsePort(childNode, deviceRef, deviceLabel, deviceImage));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			outlets.push_back(parsePort(childNode, deviceRef, deviceLabel, deviceImage));
	}
}

void OEInfo::parse(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	loaded = false;
	if (getXMLProperty(node, "version") != "1.0")
		return;
	
	// Get info properties
	label = getXMLProperty(node, "label");
	image = getXMLProperty(node, "image");
	description = getXMLProperty(node, "description");
	group = getXMLProperty(node, "group");
	
	// Make inlets and outlets
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		parseDevice(childNode);
	}
	
	// Make connections
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
	{
		OERef outletRef = getOutletRefForInletRef(doc, i->ref);
		if (!outletRef.isValid())
			continue;
		
		OEPort *o = getOutlet(outletRef);
		if (o)
		{
			i->connectedPort = &(*o);
			o->connectedPort = &(*i);
		}
		else
			cerr << "libemulator: warning: outlet \"" << outletRef <<
			"\" does not exist." << endl;
	}
	
	// Make labels
	for (OEPorts::iterator o = outlets.begin();
		 o != outlets.end();
		 o++)
	{
		vector<OERef> refs;
		o->connectedLabel = makeLabel(&(*o), &refs);
		OEPort *i = o->connectedPort;
		if (i)
			i->connectedLabel = o->connectedLabel;
	}
	
	loaded = true;
}

bool OEInfo::isLoaded()
{
	return loaded;
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

string OEInfo::getGroup()
{
	return group;
}

OEPorts *OEInfo::getInlets()
{
	return &inlets;
}

OEPorts *OEInfo::getOutlets()
{
	return &outlets;
}
