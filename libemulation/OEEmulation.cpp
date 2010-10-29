
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

OEEmulation::OEEmulation(string path, string resourcesPath) : OEInfo()
{
	this->resourcesPath = resourcesPath;
	
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
	
	if (create())
		if (configure())
			if (init())
				return true;
	
	close();
	
	return false;
}

void OEEmulation::close()
{
	destroy();
	
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

bool OEEmulation::create()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			string className = getNodeProperty(node, "class");
			
			if (!createComponent(id, className))
				return false;
		}
	
	return true;
}

bool OEEmulation::createComponent(string id, string className)
{
	if (!getComponent(id))
	{
		OEComponent *component = OEComponentFactory::create(className);
		if (component)
			return setComponent(id, component);
		else
			OELog("could not create '" + id + "', class '" + className + "' undefined");
	}
	else
		OELog("redefinition of '" + id + "'");
	
	return false;
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
		OELog("could not configure '" + id + "', it was not created");
		return false;
	}
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "value"))
			{
				string value = getNodeProperty(node, "value");
				
				if (component->setValue(name, value))
					continue;
				else
					OELog("invalid property '" + name + "' for '" + id + "'");
			}
			else if (hasNodeProperty(node, "ref"))
			{
				string ref = getNodeProperty(node, "ref");
				OEComponent *refComponent = NULL;
				
				if (ref != "")
				{
					refComponent = getComponent(ref);
					if (!refComponent)
					{
						OELog("could not set property '" + name + "' for '" +
							  id + "', ref '" + ref + "' not found");
						return false;
					}
				}
				
				if (component->setRef(name, refComponent))
					continue;
				else
					OELog("invalid property '" + name + "' for '" + id + "'");
			}
			else if (hasNodeProperty(node, "src"))
			{
				string src = getNodeProperty(node, "src");
				
				OEData *data = new OEData;
				if (data)
				{
					string parsedSrc = parseSrc(src, id);
					bool dataRead = false;
					
					if (hasSrcProperty(src, "packagePath"))
					{
						if (!package)
							continue;
						
						dataRead = package->readFile(parsedSrc, data);
					}
					else
						dataRead = readFile(parsedSrc, data);
					
					if (dataRead)
					{
						if (component->setData(name, data))
							continue;
						else
							OELog("invalid property '" + name + "' for '" + id + "'");
					}
					else
						OELog("could not read '" + src + "'");
					
					delete data;
				}
				else
					OELog("could not create OEData");
			}
			else
				OELog("invalid property '" + name + "' for '" + id +
					  "', unrecognized type");
			
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
		OELog("could not init '" + id + "', it was not created");
		return false;
	}
	
	if (component->init())
		return true;
	else
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
		OELog("could not update '" + id + "', it was not created");
		return false;
	}
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "value"))
			{
				string value;
				
				if (component->getValue(name, value))
					setNodeProperty(node, name, value);
			}
			else if (hasNodeProperty(node, "src"))
			{
				string src = getNodeProperty(node, "src");
				
				if (hasSrcProperty(src, "resourcesPath"))
					continue;
				
				OEData *data;
				string parsedSrc = parseSrc(src, id);
				
				if (component->getData(name, &data))
				{
					if (data)
					{
						if (package->writeFile(parsedSrc, data))
							continue;
						else
							OELog("could not write '" + src + "'");
						return false;
					}
				}
			}
		}
	}
	
	return true;
}

void OEEmulation::destroy()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			destroyComponent(id, node->children);
		}
	}
}

void OEEmulation::destroyComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponent(id);
	if (!component)
	{
//		OELog("could not destroy '" + id + "', it was not created");
		return;
	}
	
	delete component;
	
	setComponent(id, NULL);
}

bool OEEmulation::hasSrcProperty(string value, string property)
{
	return (value.find("${" + property + "}") != string::npos);
}

string OEEmulation::parseSrc(string src, string id)
{
	int startIndex;
	
	while ((startIndex = src.find("${")) != string::npos)
	{
		int endIndex = src.find("}", startIndex);
		if (endIndex == string::npos)
		{
			src = src.substr(0, startIndex);
			break;
		}
		
		string propertyName = src.substr(startIndex + 2,
										 endIndex - startIndex - 2);
		string replacement;
		
		if (propertyName == "id")
			replacement = id;
		else if (propertyName == "resourcesPath")
			replacement = resourcesPath;
		
		src = src.replace(startIndex,
						  endIndex - startIndex + 1,
						  replacement);
	}
	
	return src;
}
