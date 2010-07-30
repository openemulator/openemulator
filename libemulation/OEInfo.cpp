
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

bool OEInfo::removeDevice(string deviceName)
{
	if (!OEDML::removeDevice(deviceName))
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
	label = getNodeProperty(node, "label");
	image = getNodeProperty(node, "image");
	description = getNodeProperty(node, "description");
	group = getNodeProperty(node, "group");
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
	
	device->name = getName(node);
	device->type = getNodeProperty(node, "type");
	device->options = getNodeProperty(node, "options");
	device->label = getNodeProperty(node, "label");
	device->image = getNodeProperty(node, "image");
	
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
	
	setting->ref = getRef(node);
	setting->type = getNodeProperty(node, "type");
	setting->options = getNodeProperty(node, "options");
	setting->label = getNodeProperty(node, "label");
	
	device->settings.push_back(setting);
}

void OEInfo::addPort(xmlNodePtr node, OEDevice *device, OEPorts &ports)
{
	OEPort *port = new OEPort();
	if (!port)
		return;
	
	port->ref = getNodeProperty(node, "ref");
	port->type = getNodeProperty(node, "type");
	port->label = getNodeProperty(node, "label");
	port->image = getNodeProperty(node, "image");
	
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
	// Analyze connected inlets
	for (OEPorts::iterator inlet = inlets.begin();
		 inlet != inlets.end();
		 inlet++)
	{
		// Follow connection
		string deviceName = (*inlet)->device->name;
		xmlNodePtr deviceNode = getDeviceNode(deviceName);
		if (!deviceNode)
			return false;
		
		xmlNodePtr connectionNode = getConnectionNode(deviceNode, (*inlet)->ref);
		if (!connectionNode)
			return false;
		
		string connectionRef = getRef(connectionNode);
		if (connectionRef == "")
			continue;
		
		// Search outlet
		for (OEPorts::iterator outlet = outlets.begin();
			 outlet != outlets.end();
			 outlet++)
		{
			if ((*outlet)->ref == connectionRef)
			{
				if ((*outlet)->connection)
				{
					OELog("reconnection of inlet '" + (*inlet)->ref + "'");
					
					return false;
				}
				
				(*inlet)->connection = *outlet;
				(*outlet)->connection = *inlet;
			}
		}
		
		if (!(*inlet)->connection)
		{
			OELog("could not find '" + (*inlet)->ref + "'");
			
			return false;
		}
	}
	
	// Analyze disconnected outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
		if (!(*outlet)->connection)
		{
			OELog("outlet '" + (*outlet)->ref + "' is not connected");
			
			return false;
		}
	
	// Analyze connection labels
	for (OEDevices::iterator device = devices.begin();
		 device != devices.end();
		 device++)
		(*device)->connectionLabel = buildConnectionLabel((*device)->name);
	
	return true;
}

string OEInfo::buildConnectionLabel(string deviceName)
{
	string label;
	
	// Find all outlets
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		if (getDeviceName((*outlet)->ref) == deviceName)
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
	string deviceLabel = outlet->device->label;
	
	// Get the connected inlet for this outlet
	OEPort *inlet = outlet->connection;
	if (!inlet)
		return deviceLabel;
	
	string deviceName = getDeviceName(inlet->ref);
	
	// Find first outlet of the inlet's device
	for (OEPorts::iterator outlet = outlets.begin();
		 outlet != outlets.end();
		 outlet++)
	{
		if (getDeviceName((*outlet)->ref) == deviceName)
		{
			if (findCircularConnection((*outlet)->ref, visitedRefs))
				return deviceLabel;
			
			return buildConnectionLabel(*outlet, visitedRefs) + " " + inlet->label;
		}
	}
	
	// The device has no outlets
	return deviceLabel;
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
