
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

#include "OEAddress.h"

#define OE_REF_SUBST_STRING "${REF}"
#define OE_DEFAULT_RESOURCE_PATH "resources"

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
	
	if (construct())
		if (init())
			if (connect())
				return true;
	
	close();
	
	return false;
}

void OEEmulation::close()
{
	destroy();
	
	OEInfo::close();
}

OEComponent *OEEmulation::getComponent(string ref)
{
	if (!components.count(ref))
		return NULL;
	
	return components[ref];
}

bool OEEmulation::setComponent(string ref, OEComponent *component)
{
	if (components.count(ref))
		return false;
	
	components[ref] = component;
	
	return true;
}

//
// DML operations
//
bool OEEmulation::add(string path, OEConnections &connections)
{
}

bool OEEmulation::remove(string ref)
{
}

bool OEEmulation::construct()
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			if (!constructDevice(childNode))
				return false;
	
	return true;
}

bool OEEmulation::constructDevice(xmlNodePtr node)
{
	string ref = getXMLProperty(node, "name") + OE_DEVICE_SEP;
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "component"))
			if (!constructComponent(childNode, ref))
				return false;
	
	return true;
}

bool OEEmulation::constructComponent(xmlNodePtr node, string ref)
{
	string className = getXMLProperty(node, "class");
	ref = OEAddress(ref).ref(getXMLProperty(node, "name"));
	
	if (getComponent(ref))
	{
		OELog(string("redefinition of '") + ref + "'");
		
		return false;
	}
	
	OEComponent *component = OEComponentFactory::build(className);
	if (component)
		return setComponent(ref, component);
	
	OELog("could not construct '" + ref + "' (" + className + ")");
	
	return false;
}

bool OEEmulation::init()
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "device"))
			if (!initDevice(childNode))
				return false;
	
	return true;
}

bool OEEmulation::initDevice(xmlNodePtr node)
{
	OEAddress address;
	address.setDevice(getXMLProperty(node, "name"));
	string ref = address.ref();
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
		if (!xmlStrcmp(childNode->name, BAD_CAST "component"))
			if (!initComponent(childNode, ref))
				return false;
	
	return true;
}

bool OEEmulation::initComponent(xmlNodePtr node, string ref)
{
	string className = getXMLProperty(node, "class");
	ref = OEAddress(ref).ref(getXMLProperty(node, "name"));
	
	OEComponent *component = getComponent(ref);
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
			if (!setProperty(childNode, ref))
				return false;
		if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
			if (!setData(childNode, ref))
				return false;
		if (!xmlStrcmp(childNode->name, BAD_CAST "resource"))
			if (!setResource(childNode, ref))
				return false;
	}
	
	return false;
}

bool OEEmulation::connect()
{
}

void OEEmulation::update()
{
}

void OEEmulation::destroy()
{
}

//
// Component operations
//
bool OEEmulation::setProperty(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	string value = getXMLProperty(node, "value");
	
	return component->setProperty(name, value);
}

bool OEEmulation::getProperty(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	string value;
	
	if (component->getProperty(name, value))
		setXMLProperty(node, "value", value);
	
	return true;
}

bool OEEmulation::setData(xmlNodePtr node, OEComponent *component, string ref)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), ref);
	
	OEData data;
	if (!package)
		return false;
	
	if (!package->readFile(src, data))
		return true;
	
	return component->setData(name, data);
}

bool OEEmulation::getData(xmlNodePtr node, OEComponent *component, string ref)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), ref);
	
	OEData data;
	
	if (component->getData(name, data))
	{
		if (package->writeFile(src, data))
			return true;
	}
	
	return false;
}

bool OEEmulation::setResource(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	string src = resourcePath + OE_PATH_SEPARATOR + getXMLProperty(node, "src");
	
	OEData *data = new OEData();
	
	if (!readFile(src, &data))
		return false;
	
	return component->setResource(name, data);
}

bool OEEmulation::connect(xmlNodePtr node, OEComponent *component, string ref)
{
	OEAddress address(ref);
	
	string name = getXMLProperty(node, "name");
	ref = getXMLProperty(node, "ref");
	
	OEComponent *connection = NULL;
	if (refName.size())
	{
		getComponent(<#string ref#>)
		string connectionRef = componentRef.getStringRef(ref);
		if (!components.count(connectionRef))
			return false;
		
		connection = components[connectionRef];
	}
	
	return component->connect(name, connection);
}

string OEEmulation::buildSourcePath(string src, string ref)
{
	OEAddress address(ref);
	string deviceName = address.getDevice() + "_" + address.getComponent();
	
	int index = src.find(OE_REF_SUBST_STRING);
	if (index != string::npos)
		src.replace(index, sizeof(OE_REF_SUBST_STRING) - 1, deviceName);
	
	return src;
}
