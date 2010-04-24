
/**
 * libemulator
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a DML file.
 */

#include <iostream>
#include <fstream>

#include <libxml/parser.h>

#include "OEInfo.h"
#include "OEPackage.h"

OEInfo::OEInfo(string path)
{
	loaded = false;
	
	xmlDocPtr doc = loadDML(path);
	if (doc)
	{
		processDML(doc);
		
		xmlFreeDoc(doc);
	}
	else
		cerr << "OEInfo: couldn't parse" + path + "\n";
}

OEInfo::OEInfo(xmlDocPtr doc)
{
	processDML(doc);
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

string OEInfo::getXMLProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

xmlDocPtr OEInfo::loadDML(string path)
{
	vector<char> data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == "xml")
	{
		if (!readFile(path, data))
			return NULL;
	}
	else if (pathExtension == "emulation")
	{
		OEPackage package(path);
		if (package.isOpen())
		{
			if (!package.readFile(OE_INFO_FILENAME, data))
				return NULL;
		}
	}
	else
		return NULL;
	
	return xmlReadMemory(&data[0], data.size(), OE_INFO_FILENAME, NULL, 0);
}

void OEInfo::processDML(xmlDocPtr doc)
{
	loaded = false;
	
	if (!validateDML(doc))
		return;
	
	getDMLProperties(doc);
	parseDML(doc);
	setConnections(doc);
	setConnectionLabels();
	
	loaded = true;
}

void OEInfo::getDMLProperties(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	label = getXMLProperty(node, "label");
	image = getXMLProperty(node, "image");
	description = getXMLProperty(node, "description");
	group = getXMLProperty(node, "group");
}

bool OEInfo::validateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(node, "version") == "1.0");
}

void OEInfo::parseDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			parseDevice(childNode);
	}
}

void OEInfo::parseDevice(xmlNodePtr node)
{
	OERef ref = OERef(getXMLProperty(node, "name"));
	string label = getXMLProperty(node, "label");
	string image = getXMLProperty(node, "image");
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			inlets.push_back(parsePort(childNode, ref, label, image));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			outlets.push_back(parsePort(childNode, ref, label, image));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "setting"))
			settings.push_back(parseSetting(childNode, ref));
	}
}

OEPort OEInfo::parsePort(xmlNodePtr node, OERef ref, string label, string image)
{
	OEPort port;
	
	port.ref = ref.getRef(getXMLProperty(node, "ref"));
	port.type = getXMLProperty(node, "type");
	port.category = getXMLProperty(node, "category");
	port.label = getXMLProperty(node, "label");
	port.image = getXMLProperty(node, "image");
	
	port.deviceLabel = label;
	
	port.connectionPort = NULL;
	
	if (!port.label.size())
		port.label = label;
	if (!port.image.size())
		port.image = image;
	
	return port;
}

OESetting OEInfo::parseSetting(xmlNodePtr node, OERef ref)
{
	OESetting setting;
	
	setting.ref = ref.getRef(getXMLProperty(node, "ref"));
	setting.type = getXMLProperty(node, "type");
	setting.options = getXMLProperty(node, "options");
	setting.label = getXMLProperty(node, "label");
	
	return setting;
}

void OEInfo::setConnections(xmlDocPtr doc)
{
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
	{
		OERef outletRef = getOutletForInlet(doc, i->ref);
		if (outletRef.isEmpty())
			continue;
		
		OEPort *o = getPortForOutlet(outletRef);
		if (o)
		{
			i->connectionPort = &(*o);
			o->connectionPort = &(*i);
		}
		else
			cerr << "libemulator: warning: outlet \"" << outletRef <<
			"\" does not exist." << endl;
	}
}

xmlNodePtr OEInfo::getNodeForRef(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
	if (!ref.getDevice().size())
		return NULL;
	
	for(xmlNodePtr deviceNode = root->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		if (getXMLProperty(deviceNode, "name") != ref.getDevice())
			continue;
		
		if (!ref.getComponent().size())
			return deviceNode;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getXMLProperty(componentNode, "name") != ref.getComponent())
				continue;
			
			if (!ref.getProperty().size())
				return componentNode;
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getXMLProperty(connectionNode, "name") != ref.getProperty())
					continue;
				
				return connectionNode;
			}
		}
	}
	
	return NULL;
}

OERef OEInfo::getOutletForInlet(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr connectionNode = getNodeForRef(doc, ref);
	if (!connectionNode)
		return OERef();
	
	string outletStringRef = getXMLProperty(connectionNode, "ref");
	if (outletStringRef.size())
		return ref.getRef(outletStringRef);
	else
		return OERef();
}

OEPort *OEInfo::getPortForOutlet(OERef ref)
{
	for (OEPorts::iterator o = outlets.begin();
		 o != outlets.end();
		 o++)
	{
		if (ref == o->ref)
			return &(*o);
	}
	
	return NULL;
}

void OEInfo::setConnectionLabels()
{
	for (OEPorts::iterator i = inlets.begin();
		 i != inlets.end();
		 i++)
	{
		vector<OERef> refs;
		i->connectionLabel = getConnectionLabel(&(*i), refs);
		OEPort *o = i->connectionPort;
		if (o)
			o->connectionLabel = i->connectionLabel;
	}
}

string OEInfo::getConnectionLabel(OEPort *inletPort, vector<OERef> &visitedRefs)
{
	for (OEPorts::iterator o = outlets.begin();
		 o != outlets.end();
		 o++)
	{
		if (o->ref.getDevice() == inletPort->ref.getDevice())
		{
			// Avoid circular reference
			for (vector<OERef>::iterator v = visitedRefs.begin();
				 v != visitedRefs.end();
				 v++)
			{
				if (o->ref == *v)
					return inletPort->deviceLabel;
			}
			visitedRefs.push_back(o->ref);
			
			OEPort *i = o->connectionPort;
			if (!i)
				break;
			
			return getConnectionLabel(i, visitedRefs) + " " + inletPort->label;
		}
	}
	
	return inletPort->deviceLabel + " " + inletPort->label;
}
