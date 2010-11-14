
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

OEEmulation::~OEEmulation()
{
	close();
}

void OEEmulation::setResourcePath(string path)
{
	resourcePath = path;
}

bool OEEmulation::open(string path)
{
	close();
	
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
	deconfigure();
	destroy();
	
	OEInfo::close();
}

bool OEEmulation::setComponent(string id, OEComponent *component)
{
	if (component)
		components[id] = component;
	else
		components.erase(id);
	
	return true;
}

OEComponent *OEEmulation::getComponentById(string id)
{
	if (!components.count(id))
		return NULL;
	
	return components[id];
}

string OEEmulation::getIdByComponent(OEComponent *component)
{
	OEComponentsMap::iterator i;
	for (i = components.begin();
		 i != components.end();
		 i++)
		if (i->second == component)
			return i->first;

	return "";
}

string OEEmulation::getDeviceByComponent(OEComponent *component)
{
	string id = getIdByComponent(component);
	
	size_t index = id.find_last_of('.');
	if (index == string::npos)
		return "";
	else
		return id.substr(1, index - 1);
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
	if (!getComponentById(id))
	{
		OEComponent *component = OEComponentFactory::create(className);
		if (component)
			return setComponent(id, component);
		else
			log("could not create '" + id + "', class '" + className + "' undefined");
	}
	else
		log("redefinition of '" + id + "'");
	
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
	OEComponent *component = getComponentById(id);
	if (!component)
	{
		log("could not configure '" + id + "', it was not created");
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
					log("invalid property '" + name + "' for '" + id + "'");
			}
			else if (hasNodeProperty(node, "ref"))
			{
				string ref = getNodeProperty(node, "ref");
				OEComponent *refComponent = getComponentById(ref);
				if (component->setRef(name, refComponent))
					continue;
				else
					log("invalid property '" + name + "' for '" + id + "'");
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
						if (!component->setData(name, data))
							log("invalid property '" + name + "' for '" + id + "'");
						
						continue;
					}
					else
						log("could not read '" + src + "'");
					
					delete data;
				}
				else
					log("could not create OEData");
			}
			else
				log("invalid property '" + name + "' for '" + id +
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
	OEComponent *component = getComponentById(id);
	if (!component)
	{
		log("could not init '" + id + "', it was not created");
		return false;
	}
	
	if (component->init())
		return true;
	else
		log("could not init '" + id + "'");
	
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
	OEComponent *component = getComponentById(id);
	if (!component)
	{
		log("could not update '" + id + "', it was not created");
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
				
				if (hasSrcProperty(src, "resourcePath"))
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
							log("could not write '" + src + "'");
						return false;
					}
				}
			}
		}
	}
	
	return true;
}

void OEEmulation::deconfigure()
{
	if (!doc)
		return;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "component"))
		{
			string id = getNodeProperty(node, "id");
			
			deconfigureComponent(id, node->children);
		}
	}
}

void OEEmulation::deconfigureComponent(string id, xmlNodePtr children)
{
	OEComponent *component = getComponentById(id);
	if (!component)
	{
		log("could not deconfigure '" + id + "', it was not created");
		return;
	}
	
	for(xmlNodePtr node = children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "property"))
		{
			string name = getNodeProperty(node, "name");
			
			if (hasNodeProperty(node, "ref"))
				component->setRef(name, NULL);
		}
	}
}

void OEEmulation::destroy()
{
	if (!doc)
		return;
	
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
	OEComponent *component = getComponentById(id);
	if (!component)
	{
		log("could not destroy '" + id + "', it was not created");
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
		else if (propertyName == "resourcePath")
			replacement = resourcePath;
		
		src = src.replace(startIndex,
						  endIndex - startIndex + 1,
						  replacement);
	}
	
	return src;
}
