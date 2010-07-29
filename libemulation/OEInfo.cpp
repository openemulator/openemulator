
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

OEInfo::~OEInfo()
{
	removeDevices();
	removePorts(inlets);
	removePorts(outlets);
}

bool OEInfo::open(string path)
{
	removeDevices();
	removePorts(inlets);
	removePorts(outlets);
	
	if (!OEDML::open(path))
		return false;
	
	if (analyze())
		return true;
	
	close();
	
	return false;
}

bool OEInfo::add(string path, OEConnections &connections)
{
	if (!OEDML::add(path, connections))
		return false;
	
	return analyze();
}

bool OEInfo::remove(string ref)
{
	if (!OEDML::remove(ref))
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
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	analyzeHeader(node);
	analyzeDevices(node);
	return analyzeConnections();
}

void OEInfo::analyzeHeader(xmlNodePtr node)
{
	label = getXMLProperty(node, "label");
	image = getXMLProperty(node, "image");
	description = getXMLProperty(node, "description");
	group = getXMLProperty(node, "group");
}

void OEInfo::analyzeDevices(xmlNodePtr node)
{
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			addDevice(node);
}

void OEInfo::addDevice(xmlNodePtr node)
{
	OEDevice *device = new OEDevice();
	if (!device)
		return;
	
	device->ref = OEAddress(getXMLProperty(node, "name") + OE_DEVICE_SEP).address();
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
			addPort(childNode, device, inlets);
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			addPort(childNode, device, outlets);
		else if (!xmlStrcmp(childNode->name, BAD_CAST "setting"))
			addSetting(childNode, device);
	}
}

void OEInfo::addSetting(xmlNodePtr node, OEDevice *device)
{
	OESetting *setting = new OESetting();
	if (!setting)
		return;
	
	OEAddress address(device->ref);
	
	setting->ref = address.ref(getXMLProperty(node, "ref"));
	setting->type = getXMLProperty(node, "type");
	setting->options = getXMLProperty(node, "options");
	setting->label = getXMLProperty(node, "label");
	
	device->settings.push_back(setting);
}

void OEInfo::addPort(xmlNodePtr node, OEDevice *device, OEPorts &ports)
{
	OEPort *port = new OEPort();
	if (!port)
		return;
	
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
	// Analyze inlets
	for (OEPorts::iterator inlet = inlets.begin();
		 inlet != inlets.end();
		 inlet++)
	{
		// Follow inlet ref to connection
		OEAddress address((*inlet)->ref);
		xmlNodePtr connectionNode = getNode(address.ref());
		if (!connectionNode)
			return false;
		
		string connectionRef = getXMLProperty(connectionNode, "ref");
		if (connectionRef == "")
			continue;
		
		string ref = address.ref(connectionRef);
		
		// Search outlet
		for (OEPorts::iterator outlet = outlets.begin();
			 outlet != outlets.end();
			 outlet++)
		{
			if (ref == (*outlet)->ref)
			{
				if ((*outlet)->connection)
				{
					OELog(string("reconnection of inlet '") +
						  address.ref() + "'");
					
					return false;
				}
				
				(*inlet)->connection = *outlet;
				(*outlet)->connection = *inlet;
			}
		}
		
		if (!(*inlet)->connection)
		{
			OELog(string("could not find '") + ref + "'");
			
			return false;
		}
	}
	
	// Analyze disconnected outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
		if (!(*outlet)->connection)
		{
			OELog(string("outlet '") +
				  (*outlet)->ref + "' is not connected");
			
			return false;
		}
	
	// Analyze connection labels
	for (OEDevices::iterator device = devices.begin();
		 device != devices.end();
		 device++)
		(*device)->connectionLabel = buildConnectionLabel((*device)->ref);
	
	return true;
}

string OEInfo::buildConnectionLabel(string ref)
{
	string label;
	
	// Find all outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		OEAddress address((*outlet)->ref);
		if (address.getDevice() == ref)
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
			if (findCircularConnection((*outlet)->ref, visitedRefs))
				return label;
			
			return buildConnectionLabel(*outlet, visitedRefs) + " " + inlet->label;
		}
	}
	
	// The device has no outlets. Return device's label
	return label;
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
