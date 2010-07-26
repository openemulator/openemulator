
/**
 * libemulation
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a DML file.
 */

#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEInfo.h"

#include "OEAddress.h"

OEInfo::OEInfo() : OEDML()
{
}

OEInfo::OEInfo(string path) : OEDML()
{
	open(path);
}

bool OEInfo::open(string path)
{
	close();
	
	if (OEDML::open(path))
		if (analyze())
			return true;
	
	close();
	
	return false;
}

void OEInfo::close()
{
	label = "";
	image = "";
	description = "";
	group = "";
	
	// Delete devices
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
	
	// Delete inlets
	for (OEPorts::iterator inlet = inlets.begin();
		 inlet != inlets.end();
		 inlet++)
		delete *inlet;
	
	// Delete outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
		delete *outlet;
	
	OEDML::close();
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
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	// Analyze header
	label = getXMLProperty(rootNode, "label");
	image = getXMLProperty(rootNode, "image");
	description = getXMLProperty(rootNode, "description");
	group = getXMLProperty(rootNode, "group");
	
	// Analyze devices
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			analyzeDeviceNode(node);
	}
	
	// Analyze connections
	if (!analyzeConnections())
		return false;
	
	// Analyze connection labels
	for (OEDevices::iterator device = devices.begin();
		 device != devices.end();
		 device++)
		(*device)->connectionLabel = getConnectionLabel((*device)->ref);
	
	return true;
}

void OEInfo::analyzeDeviceNode(xmlNodePtr node)
{
	OEDevice *device = new OEDevice();
	
	OEAddress address;
	address.setDevice(getXMLProperty(node, "name"));
	device->ref = address.ref();
	device->type = getXMLProperty(node, "type");
	device->options = getXMLProperty(node, "options");
	device->label = getXMLProperty(node, "label");
	device->image = getXMLProperty(node, "image");
	
	device->connectionLabel = "";
	
	devices.push_back(device);
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			inlets.push_back(analyzePortNode(childNode, device));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			outlets.push_back(analyzePortNode(childNode,device));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "setting"))
			device->settings.push_back(analyzeSettingNode(childNode,
														  device->ref));
	}
}

OEPort *OEInfo::analyzePortNode(xmlNodePtr node, OEDevice *device)
{
	OEPort *port = new OEPort();
	
	OEAddress address(device->ref);
	port->ref = address.ref(getXMLProperty(node, "ref"));
	port->type = getXMLProperty(node, "type");
	port->label = getXMLProperty(node, "label");
	port->image = getXMLProperty(node, "image");
	
	if (port->label == "")
		port->label = device->label;
	if (port->image == "")
		port->image = device->image;
	
	port->connection = NULL;
	port->device = device;
	
	return port;
}

OESetting *OEInfo::analyzeSettingNode(xmlNodePtr node, string ref)
{
	OESetting *setting = new OESetting();
	
	OEAddress address(ref);
	setting->ref = address.ref(getXMLProperty(node, "ref"));
	setting->type = getXMLProperty(node, "type");
	setting->options = getXMLProperty(node, "options");
	setting->label = getXMLProperty(node, "label");
	
	return setting;
}

bool OEInfo::analyzeConnections()
{
	for (OEPorts::iterator inlet = inlets.begin();
		 inlet != inlets.end();
		 inlet++)
	{
		string ref = followRef((*inlet)->ref);
		if (ref == "")
			continue;
		
		for (OEPorts::iterator outlet = outlets.begin();
			 outlet != outlets.end();
			 outlet++)
		{
			if (ref == (*outlet)->ref)
			{
				if ((*outlet)->connection)
				{
					OELog(string("inlet \"") +
						  (*inlet)->ref + "\" is connected to more than one outlet");
					
					return false;
				}
				
				(*inlet)->connection = *outlet;
				(*outlet)->connection = *inlet;
			}
		}
		if (!(*inlet)->connection)
		{
			OELog(string("could not connect inlet \"") +
				  (*inlet)->ref + "\" to outlet \"" + ref + "\"");
			
			return false;
		}
	}
	
	return true;
}

string OEInfo::getConnectionLabel(string ref)
{
	string deviceName = OEAddress(ref).getDevice();
	string label;
	
	// Find all outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		OEAddress address((*outlet)->ref);
		if (address.getDevice() == deviceName)
		{
			vector<string> visitedRefs;
			
			if (label != "")
				label += ", ";
			label += getConnectionLabel(*outlet, visitedRefs);
		}
	}
	
	return label;
}

string OEInfo::getConnectionLabel(OEPort *outlet, vector<string> &visitedRefs)
{
	string label = outlet->device->label;
	
	// Get the connected inlet to this outlet
	OEPort *inlet = outlet->connection;
	if (!inlet)
		return label;
	
	string device = OEAddress(inlet->ref).getDevice();
	
	// Find first outlet of the inlet's device
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		OEAddress address((*outlet)->ref);
		if (address.getDevice() == device)
		{
			// Circular reference check
			for (vector<string>::iterator visitedRef = visitedRefs.begin();
				 visitedRef != visitedRefs.end();
				 visitedRef++)
			{
				if ((*outlet)->ref == *visitedRef)
					return label;
			}
			visitedRefs.push_back((*outlet)->ref);
			
			return getConnectionLabel(*outlet, visitedRefs) + " " + inlet->label;
		}
	}
	
	// The device has no outlets. Return device's label
	return label;
}
