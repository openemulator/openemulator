
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
		if (configure())
			if (init())
				return true;
	
	close();
	
	return false;
}

void OEEmulation::close()
{
	remove();
	
	OEInfo::close();
}

OEComponent *OEEmulation::getComponent(string id)
{
	if (!componentsMap.count(id))
		return NULL;
	
	return componentsMap[id];
}

bool OEEmulation::setComponent(string id, OEComponent *component)
{
	if (component)
		componentsMap[id] = component;
	else
		componentsMap.erase(id);
	
	return true;
}

bool OEEmulation::hasProperty(string value, string property)
{
	return (value.find("${" + property + "$}") != string::npos);
}

string OEEmulation::parseProperties(string value, string id)
{
	int startIndex;
	
	while ((startIndex = value.find("${")) != string::npos)
	{
		int endIndex = value.find("}", startIndex);
		if (endIndex == string::npos)
		{
			value = value.substr(0, startIndex);
			break;
		}
		
		string propertyName = value.substr(startIndex + 2,
										   endIndex - startIndex - 3);
		string replacement;
		
		if (propertyName == "id")
			replacement = id;
		else if (propertyName == "resourcePath")
			replacement = resourcePath;
		
		value = value.replace(startIndex,
							  endIndex - startIndex + 1,
							  replacement);
	}
	
	return value;
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
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			string className = getNodeProperty(node, "class");
			
			if (!buildComponent(id, className))
				return false;
		}
	
	return true;
}

bool OEEmulation::buildComponent(string id, string className)
{
	if (getComponent(id))
	{
		OELog("redefinition of '" + id + "'");
		
		return false;
	}
	
	OEComponent *component = OEComponentFactory::build(className);
	if (!component)
	{
		OELog("could not build '" + id + "', class '" + className + "'");
		
		return false;
	}
	
	return setComponent(id, component);
}

bool OEEmulation::configure()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!configureComponent(id, node->children))
				return false;
		}
	
	return true;
}

bool OEEmulation::configureComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		OELog("could not configure '" + id + "', it was not defined");
		return false;
	}
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			string value, ref, src;
			
			if ((value = getNodeProperty(node, "value")) != "")
			{
				if (component->setValue(name, value))
					continue;
			}
			else if ((ref = getNodeProperty(node, "ref")) != "")
			{
				OEComponent *refComponent = getComponent(ref);
				if (!refComponent)
				{
					OELog("could not set property '" + name + "' for '" +
						  id + "', ref not found");
					return false;
				}
				
				if (component->setComponent(name, refComponent))
					continue;
			}
			else if ((src = getNodeProperty(node, "src")) != "")
			{
				src = parseProperties(src, id);
				
				OEData *data = new OEData;
				if (data)
				{
					if (package->readFile(src, data))
					{
						if (component->setData(name, data))
							continue;
					}
					else
						OELog("could not find '" + src + "'");
				}
				else
					OELog("could not create OEData");
				
				delete data;
			}
			
			OELog("could not set property '" + name + "' for '" + id + "'");
			
			return false;
		}
	}
	
	return true;
}

bool OEEmulation::init()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!initComponent(id))
				return false;
		}
	
	return true;
}

bool OEEmulation::initComponent(string id)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		OELog("could not init '" + id + "', it was not defined");
		return false;
	}
	
	if (component->init())
		return true;
	
	OELog("could not init '" + id + "'");
	
	return false;
}

bool OEEmulation::update()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			if (!updateComponent(id, node->children))
				return false;
		}
	
	return true;
}

bool OEEmulation::updateComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		OELog("could not update '" + id + "', it was not defined");
		return false;
	}
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			string value, ref, src;
			
			if ((value = getNodeProperty(node, "value")) != "")
			{
				if (component->getValue(name, value))
					setNodeProperty(node, name, value);
			}
			else if ((src = getNodeProperty(node, "src")) != "")
			{
				if (hasProperty(src, "resourcePath"))
					continue;
				
				src = parseProperties(src, id);
				OEData *data;
				
				if (component->getData(name, &data))
				{
					if (data)
					{
						if (package->writeFile(src, data))
							continue;
						
						OELog("could not write '" + src + "'");
						return false;
					}
				}
			}
		}
	}
	
	return true;
}

void OEEmulation::remove()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			removeComponent(id, node->children);
		}
	}
}

void OEEmulation::removeComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
		OELog("could not remove '" + id + "', it was not defined");
		return;
	}
	
	delete component;
	
	setComponent(id, NULL);
}
