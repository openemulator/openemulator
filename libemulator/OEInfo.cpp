
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
	if (pathExtension == "xml")
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

OESplitRef OEInfo::getSplitRef(string ref)
{
	OESplitRef name;
	
	int deviceIndex = ref.find(OE_DEVICE_SEP);
	if (deviceIndex == string::npos)
	{
		name.device = ref;
		return name;
	}
	
	name.device = ref.substr(0, deviceIndex);
	deviceIndex += sizeof(OE_DEVICE_SEP) - 1;
	
	int componentIndex = ref.find(OE_COMPONENT_SEP, deviceIndex);
	if (componentIndex == string::npos)
	{
		name.component = ref.substr(deviceIndex);
		name.property = "";
		return name;
	}

	name.component = ref.substr(deviceIndex, componentIndex - deviceIndex);
	componentIndex += sizeof(OE_COMPONENT_SEP) - 1;
	name.property = ref.substr(componentIndex);

	return name;
}

string OEInfo::getConnectedOutletRef(xmlDocPtr dml, string inletRef)
{
	OESplitRef splitRef = getSplitRef(inletRef);
	
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		if (getNodeProperty(deviceNode, "name") != splitRef.device)
			continue;
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			if (getNodeProperty(componentNode, "name") != splitRef.component)
				continue;
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				if (getNodeProperty(connectionNode, "key") != splitRef.property)
					continue;
				
				string outletRef = getNodeProperty(connectionNode, "ref");
				return buildAbsoluteRef(splitRef.device, outletRef);
			}
		}
	}
	
	return "";
}

OEPortInfo *OEInfo::getOutletProperties(string outletRef)
{
	for (OEPortsInfo::iterator o = outletsInfo.begin();
		 o != outletsInfo.end();
		 o++)
	{
		if (o->ref == outletRef)
			return &(*o);
	}
	
	return NULL;
}

string OEInfo::buildConnectedLabel(OEPortInfo *outlet, vector<string> *refList)
{
	OEPortInfo *inlet = outlet->connectedPort;
	if (!inlet)
		return "Unknown";
	OESplitRef iSplitRef = getSplitRef(inlet->ref);
	
	// For some odd reason, g++ does not like the next line in the loop
	OESplitRef oSplitRef;
	for (OEPortsInfo::iterator o = outletsInfo.begin();
		 o != outletsInfo.end();
		 o++)
	{
		oSplitRef = getSplitRef(o->ref);
		if (iSplitRef.device != oSplitRef.device)
			continue;
		
		for (vector<string>::iterator r = refList->begin();
			 r != refList->end();
			 r++)
		{
			if (*r == o->ref)
				return inlet->label;
		}
		refList->push_back(o->ref);
		
		return buildConnectedLabel(&(*o), refList) + " " + inlet->label;
	}
	
	return inlet->deviceLabel + " " + inlet->label;
}

string OEInfo::buildConnectedLabel(OEPortInfo *outlet)
{
	vector<string> refList;
	string test = buildConnectedLabel(outlet, &refList);
	return test;
}

bool OEInfo::parse(xmlDocPtr dml)
{
	xmlNodePtr dmlNode = xmlDocGetRootElement(dml);
	
	if (getNodeProperty(dmlNode, "version") != "1.0")
		return false;
	
	// Set DML properties
	dmlInfo.label = getNodeProperty(dmlNode, "label");
	dmlInfo.image = getNodeProperty(dmlNode, "image");
	dmlInfo.description = getNodeProperty(dmlNode, "description");
	dmlInfo.group = getNodeProperty(dmlNode, "group");
	
	// Build inlets and outlets
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getNodeProperty(deviceNode, "name");
		string deviceLabel = getNodeProperty(deviceNode, "label");
		string deviceImage = getNodeProperty(deviceNode, "image");
		
		for(xmlNodePtr node = deviceNode->children;
			node;
			node = node->next)
		{
			if (!xmlStrcmp(node->name, BAD_CAST "inlet"))
				inletsInfo.push_back(parsePort(deviceName,
											   deviceLabel,
											   deviceImage,
											   node));
			else if (!xmlStrcmp(node->name, BAD_CAST "outlet"))
				outletsInfo.push_back(parsePort(deviceName,
												deviceLabel,
												deviceImage,
												node));
		}
	}
	
	// Find connectedPort's
	for (OEPortsInfo::iterator i = inletsInfo.begin();
		 i != inletsInfo.end();
		 i++)
	{
		string outletRef = getConnectedOutletRef(dml, i->ref);
		if (!outletRef.size())
			continue;
		
		OEPortInfo *o = getOutletProperties(outletRef);
		
		if (o)
		{
			i->connectedPort = &(*o);
			o->connectedPort = &(*i);
		}
		else
			fprintf(stderr,
					"libemulator: warning, outlet \"%s\" does not exist.\n",
					outletRef.c_str());
	}
	
	// Build connectedLabel's
	for (OEPortsInfo::iterator o = outletsInfo.begin();
		 o != outletsInfo.end();
		 o++)
	{
		o->connectedLabel = buildConnectedLabel(&(*o));
		OEPortInfo *i = o->connectedPort;
		if (i)
			i->connectedLabel = o->connectedLabel;
	}
	
	return true;
}

OEPortInfo OEInfo::parsePort(string deviceName, 
								   string deviceLabel,
								   string deviceImage,
								   xmlNodePtr node)
{
	OEPortInfo prop;
	
	prop.ref = buildAbsoluteRef(deviceName, getNodeProperty(node, "ref"));
	prop.type = getNodeProperty(node, "type");
	prop.subtype = getNodeProperty(node, "subtype");
	prop.label = getNodeProperty(node, "label");
	prop.image = getNodeProperty(node, "image");
	
	prop.deviceLabel = deviceLabel;
	
	prop.connectedPort = NULL;
	
	if (!prop.label.size())
		prop.label = deviceLabel;
	if (!prop.image.size())
		prop.image = deviceImage;
	
	return prop;
}

bool OEInfo::isOpen()
{
	return open;
}

OEDMLInfo *OEInfo::getDMLInfo()
{
	return &dmlInfo;
}

vector<OEPortInfo> *OEInfo::getInletsInfo()
{
	return &inletsInfo;
}

vector<OEPortInfo> *OEInfo::getOutletsInfo()
{
	return &outletsInfo;
}
