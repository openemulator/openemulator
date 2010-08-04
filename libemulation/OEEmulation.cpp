
/**
 * libemulation
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEComponentFactory.h"

OEEmulation::OEEmulation() : OEInfo()
{
}

OEEmulation::OEEmulation(string path) : OEInfo()
{
}

OEEmulation::OEEmulation(string path, string resourcePath) : OEInfo()
{
	this->resourcePath = resourcePath;
	
	open(path);
}

OEEmulation::~OEEmulation()
{
	close();
}

bool OEEmulation::open(string path)
{
	if (!OEInfo::open(path))
		return false;
	
	if (build())
		if (init())
			if (connect())
				return true;
	
	close();
	
	return false;
}

void OEEmulation::close()
{
	remove();
	
	OEInfo::close();
}

OEComponent *OEEmulation::getComponent(string ref)
{
	if (!componentsMap.count(ref))
		return NULL;
	
	return componentsMap[ref];
}

bool OEEmulation::setComponent(string ref, OEComponent *component)
{
	if (component)
		componentsMap[ref] = component;
	else
		componentsMap.erase(ref);
	
	return true;
}

//
// Operations
//
bool OEEmulation::build()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			if (!buildDevice(node))
				return false;
	
	return true;
}

bool OEEmulation::buildDevice(xmlNodePtr deviceNode)
{
	string deviceName = getName(deviceNode);
	
	for(xmlNodePtr node = deviceNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
			if (!buildComponent(node, deviceName))
				return false;
	
	return true;
}

bool OEEmulation::buildComponent(xmlNodePtr componentNode, string deviceName)
{
	string className = getNodeProperty(componentNode, "class");
	string ref = deviceName + OE_DEVICE_SEPARATOR + getName(componentNode);
	
	if (getComponent(ref))
	{
		OELog("redefinition of '" + ref + "'");
		
		return false;
	}
	
	OEComponent *component = OEComponentFactory::build(className);
	if (!component)
	{
		OELog("could not build '" + ref + "', class '" + className +
			  "' was not declared");
		
		return false;
	}
	
	return setComponent(ref, component);
}

bool OEEmulation::init()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			if (!initDevice(node))
				return false;
	
	return true;
}

bool OEEmulation::initDevice(xmlNodePtr deviceNode)
{
	string deviceName = getName(deviceNode);
	
	for(xmlNodePtr node = deviceNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
			if (!initComponent(node, deviceName))
				return false;
	
	return true;
}

bool OEEmulation::initComponent(xmlNodePtr componentNode, string deviceName)
{
	string ref = deviceName + OE_DEVICE_SEPARATOR + getName(componentNode);
	OEComponent *component = getComponent(ref);
	
	for(xmlNodePtr node = componentNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
			if (!setProperty(node, component, ref))
				return false;
		if (!xmlStrcmp(node->name, BAD_CAST "data"))
			if (!setData(node, component, ref))
				return false;
		if (!xmlStrcmp(node->name, BAD_CAST "resource"))
			if (!setResource(node, component, ref))
				return false;
	}
	
	return true;
}

bool OEEmulation::connect()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			if (!connectDevice(node))
				return false;
	
	return true;
}

bool OEEmulation::connectDevice(xmlNodePtr deviceNode)
{
	string deviceName = getName(deviceNode);
	
	for(xmlNodePtr node = deviceNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
			if (!connectComponent(node, deviceName))
				return false;
	
	return true;
}

bool OEEmulation::connectComponent(xmlNodePtr componentNode, string deviceName)
{
	string ref = deviceName + OE_DEVICE_SEPARATOR + getName(componentNode);
	OEComponent *component = getComponent(ref);
	
	for(xmlNodePtr node = componentNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "connection"))
			if (!connect(node, component, ref))
				return false;
	}
	
	return true;
}

void OEEmulation::update()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			updateDevice(node);
}

void OEEmulation::updateDevice(xmlNodePtr deviceNode)
{
	string deviceName = getName(deviceNode);
	
	for(xmlNodePtr node = deviceNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
			updateComponent(node, deviceName);
}

void OEEmulation::updateComponent(xmlNodePtr componentNode, string deviceName)
{
	string ref = deviceName + OE_DEVICE_SEPARATOR + getName(componentNode);
	OEComponent *component = getComponent(ref);
	
	for(xmlNodePtr node = componentNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
			getProperty(node, component, ref);
		if (!xmlStrcmp(node->name, BAD_CAST "data"))
			getData(node, component, ref);
	}
}

void OEEmulation::remove()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	if (!rootNode)
		return;
	
	// To-Do: Fix deletion for floating memory access
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
			removeDevice(node);
}

void OEEmulation::removeDevice(xmlNodePtr deviceNode)
{
	string deviceName = getName(deviceNode);
	
	for(xmlNodePtr node = deviceNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
			removeComponent(node, deviceName);
	
//	OEDML::removeDevice(deviceNode);
}

void OEEmulation::removeComponent(xmlNodePtr componentNode, string deviceName)
{
	string ref = deviceName + OE_DEVICE_SEPARATOR + getName(componentNode);
	
	delete getComponent(ref);
	setComponent(ref, NULL);
}

//
// Operations
//
bool OEEmulation::setProperty(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string value = getNodeProperty(componentNode, "value");
	
	if (component->setProperty(name, value))
		return true;
	
	OELog("could not set property '" + ref + "." + name + "'");
	
	return false;
}

void OEEmulation::getProperty(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string value;
	
	if (component->getProperty(name, value))
		setNodeProperty(componentNode, "value", value);
}

bool OEEmulation::setData(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string src = replaceRef(getNodeProperty(componentNode, "src"), ref);
	
	if (!package)
		return false;
	
	OEData *data = new OEData;
	if (!data)
		return false;
	
	if (!package->readFile(src, data))
		return true;
	
	if (component->setData(name, data))
		return true;
	
	OELog("could not set data '" + ref + "." + name + "'");
	delete data;
	
	return false;
}

void OEEmulation::getData(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string src = replaceRef(getNodeProperty(componentNode, "src"), ref);
	
	OEData *data;
	
	if (!package)
		return;
	
	if (!component->getData(name, &data))
		return;
	
	package->writeFile(src, data);
}

bool OEEmulation::setResource(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string path = (resourcePath + OE_PATH_SEPARATOR +
				   getNodeProperty(componentNode, "src"));
	
	OEData *data = new OEData;
	if (!data)
		return false;
	
	if (!readFile(path, data))
	{
		OELog("could not read resource '" + path + "'");
		
		return false;
	}
	
	if (component->setResource(name, data))
		return true;
	
	delete data;
	OELog("could not set resource '" + ref + "." + name + "'");
	
	return false;
}

bool OEEmulation::connect(xmlNodePtr componentNode, OEComponent *component, string ref)
{
	string name = getName(componentNode);
	string connectionRef = getRef(componentNode);
	
	OEComponent *connection = NULL;
	if (connectionRef != "")
	{
		if (getDeviceName(connectionRef) == "")
			connectionRef = getDeviceName(ref) + OE_DEVICE_SEPARATOR + connectionRef;
	
		connection = getComponent(connectionRef);
		
		if (!connection)
			OELog("could not connect '" + ref + "." + name +
				  "', reference was not declared");
	}
	
	if (component->connect(name, connection))
		return true;
	
	OELog("could not connect '" + ref + "." + name + "'");
	
	return false;
}

string OEEmulation::replaceRef(string src, string ref)
{
	for (int i = 0; i < ref.size(); i++)
		if (ref[i] == OE_DEVICE_SEPARATOR[0])
			ref[i] = '_';
	
	int index = src.find(OE_REPLACE_REF_STRING);
	if (index != string::npos)
		src.replace(index, sizeof(OE_REPLACE_REF_STRING) - 1, ref);
	
	return src;
}
