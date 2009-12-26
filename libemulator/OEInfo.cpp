
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
#include "Package.h"

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
		Package package(path);
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
	if (dml)
	{
		parse(dml);
		xmlFreeDoc(dml);
	}
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

string OEInfo::getNodeProperty(xmlNodePtr node, string key)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST key.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
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
		
		if (getNodeProperty(deviceNode, "name") != inletRef.getDevice())
			continue;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getNodeProperty(componentNode, "name") != inletRef.getComponent())
				continue;
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getNodeProperty(connectionNode, "key") != inletRef.getProperty())
					continue;
				
				string stringRef = getNodeProperty(connectionNode, "ref");
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

OEPort OEInfo::parsePort(OERef deviceRef,
						 string deviceLabel,
						 string deviceImage,
						 xmlNodePtr node)
{
	OEPort port;
	
	port.ref = deviceRef.buildRef(getNodeProperty(node, "ref"));
	port.type = getNodeProperty(node, "type");
	port.subtype = getNodeProperty(node, "subtype");
	port.label = getNodeProperty(node, "label");
	port.image = getNodeProperty(node, "image");
	
	port.deviceLabel = deviceLabel;
	
	port.connectedPort = NULL;
	
	if (!port.label.size())
		port.label = deviceLabel;
	if (!port.image.size())
		port.image = deviceImage;
	
	// To-do: parse settings
	
	return port;
}

void OEInfo::parse(xmlDocPtr dml)
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	loaded = false;
	if (getNodeProperty(dmlNode, "version") != "1.0")
		return;
	
	// Get info properties
	label = getNodeProperty(dmlNode, "label");
	image = getNodeProperty(dmlNode, "image");
	description = getNodeProperty(dmlNode, "description");
	group = getNodeProperty(dmlNode, "group");
	
	// Make inlets and outlets
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		OERef deviceRef = OERef(getNodeProperty(deviceNode, "name"));
		string deviceLabel = getNodeProperty(deviceNode, "label");
		string deviceImage = getNodeProperty(deviceNode, "image");
		
		for(xmlNodePtr node = deviceNode->children;
			node;
			node = node->next)
		{
			if (!xmlStrcmp(node->name, BAD_CAST "inlet"))
				inlets.push_back(parsePort(deviceRef,
										   deviceLabel,
										   deviceImage,
										   node));
			else if (!xmlStrcmp(node->name, BAD_CAST "outlet"))
				outlets.push_back(parsePort(deviceRef,
											deviceLabel,
											deviceImage,
											node));
		}
	}
	
	// Make connections
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
	{
		OERef outletRef = getOutletRefForInletRef(dml, i->ref);
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
