
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

bool OEInfo::open(string path)
{
	close();
	
	if (OEDML::open(path))
	{
		analyze();
		analyzeConnections();
		analyzeLabels();
		
		return true;
	}
	
	close();
	
	return false;
}

void OEInfo::close()
{
	label = "";
	image = "";
	description = "";
	group = "";
	
	inlets.clear();
	outlets.clear();
	settings.clear();

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

OEPorts *OEInfo::getInlets()
{
	return &inlets;
}

OEPorts *OEInfo::getOutlets()
{
	return &outlets;
}

OESettings *OEInfo::getSettings()
{
	return &settings;
}

//
// Analysis
//

void OEInfo::analyze()
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	label = getXMLProperty(node, "label");
	image = getXMLProperty(node, "image");
	description = getXMLProperty(node, "description");
	group = getXMLProperty(node, "group");
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			analyzeDevice(childNode);
	}
}

void OEInfo::analyzeDevice(xmlNodePtr node)
{
	OERef ref = OERef(getXMLProperty(node, "name"));
	string label = getXMLProperty(node, "label");
	string image = getXMLProperty(node, "image");
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			inlets.push_back(analyzePort(childNode, ref, label, image));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			outlets.push_back(analyzePort(childNode, ref, label, image));
		else if (!xmlStrcmp(childNode->name, BAD_CAST "setting"))
			settings.push_back(analyzeSetting(childNode, ref));
	}
}

OEPort OEInfo::analyzePort(xmlNodePtr node, OERef ref, string label, string image)
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

OESetting OEInfo::analyzeSetting(xmlNodePtr node, OERef ref)
{
	OESetting setting;
	
	setting.ref = ref.getRef(getXMLProperty(node, "ref"));
	setting.type = getXMLProperty(node, "type");
	setting.options = getXMLProperty(node, "options");
	setting.label = getXMLProperty(node, "label");
	
	return setting;
}

void OEInfo::analyzeConnections()
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
			OELog("could not connect inlet \"" + i->ref.getStringRef() +
				  "\" to outlet \"" + outletRef.getStringRef() + "\"");
	}
}

void OEInfo::analyzeLabels()
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

xmlNodePtr OEInfo::getNodeForRef(xmlDocPtr dml, OERef ref)
{
	xmlNodePtr root = xmlDocGetRootElement(dml);
	
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

OERef OEInfo::getOutletForInlet(xmlDocPtr dml, OERef ref)
{
	xmlNodePtr connectionNode = getNodeForRef(dml, ref);
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
