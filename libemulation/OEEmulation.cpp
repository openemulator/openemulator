
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
	close();
	
	if (OEDML::open(path))
	{
		if (iterate(constructDevice))
		{
			if (iterate(initDevice))
			{
				if (iterate(connectDevice))
					return true;
			}
		}
	}
	
	close();
	
	return false;
}

void OEEmulation::close()
{
	iterate(destroyDevice);
	
	OEInfo::close();
}

OEComponent *OEEmulation::getComponent(string ref)
{
	if (!components.count(ref))
		return NULL;
	
	return components[ref];
}

bool OEEmulation::addDevices(string path, OEStringRefMap connections)
{
/*	OEEmulation = new OEEmulation(path);
	if (!isLoaded())
		return false;
	
	vector<char> data;
	if (!readFile(path, data))
	{
		OELog("could not read \"" + path + "\"");
		return false;
	}
	
	xmlDocPtr doc;
	doc = xmlReadMemory(&data[0],
						data.size(),
						OE_INFO_FILENAME,
						NULL,
						0);
	if (!doc)
	{
		OELog("could not parse \"" + path + "\"");
		return false;
	}
	
	bool success = false;
	
	if (validateDML(doc))
	{
		map<string, string> deviceNameMap;
		buildDeviceNameMap(documentDML, doc, deviceNameMap);
		renameDMLConnections(doc, connections, deviceNameMap);
		
		if (constructDML(doc))
		{
			if (initDML(doc))
			{
				OERef firstInletRef;
				OEStringRefMap::iterator i = connections.begin();
				if (i != connections.end())
					firstInletRef = OERef(i->first);
				
				if (insertDoc(getNodeOfPreviousInlet(documentDML, firstInletRef), doc))
				{
					if (connectDevices(documentDML, connections))
						success = true;
					else
						OELog("could not connect \"" + path + "\"");
				}
				else
					OELog("could not insert \"" + path + "\"");
			}
			else
				OELog("could not initialize \"" + path + "\"");
		}
		else
			OELog("could not construct \"" + path + "\"");
	}
	else
		OELog("could not validate \"" + path + "\"");
	
	xmlFreeDoc(doc);
	
	return success;*/
}

bool OEEmulation::removeDevice(OERef ref)
{
/*	if (!isLoaded())
		return false;
	
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(documentDML, deviceRef);
	if (!deviceNode)
	{
		OELog("could not find device node for \"" + ref.getStringRef() + "\"");
		return false;
	}
	
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		OERef outletRef = getOutletForInlet(documentDML, inletRef);
		if (outletRef.isEmpty())
			continue;
		
		if (!removeDevice(outletRef))
		{
			OELog("could not remove device at outlet \"" +
				  outletRef.getStringRef() + "\"");
			return false;
		}
	}
	
	if (!disconnectDevice(documentDML, deviceRef))
	{
		OELog("could not disconnect \"" + ref.getStringRef() + "\"");
		return false;
	}
	
	if (deviceNode->next && (deviceNode->next->type == XML_TEXT_NODE))
	{
		destroyDevice(deviceNode->next);
		xmlUnlinkNode(deviceNode->next);
	}
	
	destroyDevice(deviceNode);
	xmlUnlinkNode(deviceNode);
	xmlFreeNode(deviceNode);
	
	return true;*/
}


















//
// DML Operations
//
bool OEEmulation::iterate(bool (OEEmulation::*callback)(xmlNodePtr node))
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!(this->*callback)(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::iterateDevice(xmlNodePtr node,
								bool (OEEmulation::*callback)(xmlNodePtr node, OERef ref))
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!(this->*callback)(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::constructDevice(xmlNodePtr node)
{
	return iterateDevice(node, constructComponent);
}

bool OEEmulation::initDevice(xmlNodePtr node)
{
	return iterateDevice(node, initComponent);
}

bool OEEmulation::connectDevice(xmlNodePtr node)
{
	return iterateDevice(node, connectComponent);
}

bool OEEmulation::updateDevice(xmlNodePtr node)
{
	return iterateDevice(node, updateComponent);
}

bool OEEmulation::destroyDevice(xmlNodePtr node)
{
	return iterateDevice(node, destroyComponent);
}

bool OEEmulation::constructComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentClass = getXMLProperty(node, "class");
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	OEComponent *component = OEComponentFactory::build(string(componentClass));
	if (!component)
	{
		OELog("could not construct \"" + componentRef + "\" (" +
			  componentClass + ")");
		return false;
	}
	
	if (components.count(componentRef))
		components.erase(componentRef);
	components[componentRef] = component;
	
	return true;
}

bool OEEmulation::initComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	if (!components.count(componentRef))
	{
		OELog("could not find " + componentRef);
		
		return false;
	}
	
	OEComponent *component = components[componentRef];
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!setProperty(childNode, component))
			{
				OELog("could not set property " + componentRef + "." +
					  getXMLProperty(childNode, "name"));
				
				return false;
			}
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
			if (!setData(childNode, component, componentRef))
			{
				OELog("could not set data " + componentRef + "." +
					  getXMLProperty(childNode, "name"));
				
				return false;
			}
			else if (!xmlStrcmp(childNode->name, BAD_CAST "resource"))
			{
				if (!setResource(childNode, component))
				{
					OELog("could not set resource " + componentRef + "." +
						  getXMLProperty(childNode, "name"));
					
					return false;
				}
			}
	}
	
	return true;
}

bool OEEmulation::connectComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	if (!components.count(componentRef))
	{
		OELog("could not find " + componentRef);
		
		return false;
	}
	
	OEComponent *component = components[componentRef];
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "connection"))
		{
			if (!setConnection(childNode, component, componentRef))
			{
				OELog("could not connect " + componentRef + "." +
					  getXMLProperty(childNode, "name"));
				
				return false;
			}
		}
	}
	
	return true;
}

bool OEEmulation::updateComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	if (!components.count(componentRef))
	{
		OELog("could not find " + componentRef);
		
		return false;
	}
	
	OEComponent *component = components[componentRef];
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!getProperty(childNode, component))
			{
				OELog("could not get property " + componentRef + "." +
					  getXMLProperty(childNode, "name"));
				
				return false;
			}
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
		{
			if (!getData(childNode, component, componentRef))
			{
				OELog("could not get data " + componentRef + "." +
					  getXMLProperty(childNode, "name"));
				
				return false;
			}
		}
	}
	
	return true;
}

void OEEmulation::destroyComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	if (!components.count(componentRef))
	{
		OELog("could not find " + componentRef);
		
		return;
	}	
	components.erase(componentRef);
}

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

bool OEEmulation::setData(xmlNodePtr node, OEComponent *component, OERef componentRef)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), componentRef);
	
	OEData data;
	if (!package)
		return false;
	
	if (!package->readFile(src, data))
		return true;
	
	return component->setData(name, data);
}

bool OEEmulation::getData(xmlNodePtr node, OEComponent *component, OERef componentRef)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), componentRef);
	
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
	
	OEData data;
	
	if (!readFile(src, data))
		return false;
	
	return component->setResource(name, data);
}

bool OEEmulation::setConnection(xmlNodePtr node, OEComponent *component, OERef componentRef)
{
	string name = getXMLProperty(node, "name");
	string ref = getXMLProperty(node, "ref");
	
	OEComponent *connection = NULL;
	if (ref.size())
	{
		string connectionRef = componentRef.getStringRef(ref);
		if (!components.count(connectionRef))
			return false;
		
		connection = components[connectionRef];
	}
	
	return component->connect(name, connection);
}

xmlNodePtr OEEmulation::getNodeOfFirstInlet(xmlDocPtr doc, OERef ref)
{
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
	if (!deviceNode)
		return NULL;
	
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		OERef outletRef = getOutletForInlet(documentDML, inletRef);
		return getNodeForRef(documentDML, outletRef);
	}
	
	return NULL;
}

xmlNodePtr OEEmulation::getNodeOfLastInlet(xmlDocPtr doc, OERef ref, vector<OERef> &visitedRefs)
{
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
	if (!deviceNode)
		return NULL;
	
	// Avoid circular reference
	for (vector<OERef>::iterator v = visitedRefs.begin();
		 v != visitedRefs.end();
		 v++)
	{
		if (deviceRef == *v)
			return NULL;
	}
	visitedRefs.push_back(deviceRef);
	
	OERef lastRef;
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		OERef outletRef = getOutletForInlet(doc, inletRef);
		if (!outletRef.isEmpty())
			lastRef = outletRef;
	}
	
	if (lastRef.isEmpty())
		return deviceNode;
	
	return getNodeOfLastInlet(doc, lastRef, visitedRefs);
}

xmlNodePtr OEEmulation::getNodeOfPreviousInlet(xmlDocPtr doc, OERef ref)
{
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(doc, deviceRef);
	if (!deviceNode)
		return xmlDocGetRootElement(doc);
	
	OERef prevRef;
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		if (inletRef == ref)
		{
			if (prevRef.isEmpty())
				return deviceNode;
			else
			{
				vector<OERef> visitedRefs;
				return getNodeOfLastInlet(doc, prevRef, visitedRefs);
			}
		}
		
		OERef outletRef = getOutletForInlet(doc, inletRef);
		if (!outletRef.isEmpty())
			prevRef = outletRef;
	}
	
	return deviceNode;
}

void OEEmulation::buildDeviceNameMap(xmlDocPtr doc, xmlDocPtr elem,
									 OEStringRefMap &deviceNameMap)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(elem);
	
	int devIndex = 0;
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getXMLProperty(childNode, "name");
		
		string newDeviceName;
		do {
			newDeviceName = "dev" + getString(devIndex);
			devIndex++;
		} while (getNodeForRef(doc, OERef(newDeviceName)));
		deviceNameMap[deviceName] = newDeviceName;
	}
}

void OEEmulation::renameDMLConnections(xmlDocPtr doc,
									   OEStringRefMap &connections,
									   OEStringRefMap &deviceNameMap)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr deviceNode = rootNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		string deviceName = getXMLProperty(deviceNode, "name");
		OERef deviceRef(deviceName);
		setXMLProperty(deviceNode, "name", deviceNameMap[deviceName]);
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			OERef componentRef = deviceRef.getRef(getXMLProperty(componentNode, "name"));
			
			for(xmlNodePtr node = componentNode->children;
				node;
				node = node->next)
			{
				if (xmlStrcmp(node->name, BAD_CAST "connection"))
					continue;
				
				string outletStringRef = getXMLProperty(node, "ref");
				if (!outletStringRef.size())
					continue;
				
				OERef outletRef = componentRef.getRef(outletStringRef);
				string outletDeviceName = outletRef.getDevice();
				
				if (deviceNameMap.count(outletDeviceName))
				{
					outletRef.setDevice(deviceNameMap[outletDeviceName]);
					setXMLProperty(node, "ref", outletRef.getStringRef());
				}
			}
		}
	}
	
	for (OEStringRefMap::iterator i = connections.begin();
		 i != connections.end();
		 i++)
	{
		OERef outletRef = OERef(i->second);
		outletRef.setDevice(deviceNameMap[outletRef.getDevice()]);
		connections[i->first] = outletRef.getStringRef();
	}
}

bool OEEmulation::connectDevices(xmlDocPtr doc, OEStringRefMap &connections)
{
	for (OEStringRefMap::iterator i = connections.begin();
		 i != connections.end();
		 i++)
	{
		OERef inletRef = OERef(i->first);
		OERef deviceRef = inletRef.getDeviceRef();
		xmlNodePtr node = getNodeForRef(doc, inletRef);
		if (!node)
			return false;
		
		string stringRef = inletRef.getComponentRef().getStringRef();
		if (!components.count(stringRef))
			return false;
		OEComponent *component = components[stringRef];
		
		setXMLProperty(node, "ref", i->second);
		if (!setConnection(node, component, deviceRef))
		{
			OELog("could not connect " + stringRef + "." +
				  getXMLProperty(node, "name"));
			
			return false;
		}
	}
	
	return true;
}

bool OEEmulation::disconnectDevice(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr deviceNode = rootNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		if (xmlStrcmp(deviceNode->name, BAD_CAST "device"))
			continue;
		
		OERef deviceRef(getXMLProperty(deviceNode, "name"));
		
		for(xmlNodePtr componentNode = deviceNode->children;
			componentNode;
			componentNode = componentNode->next)
		{
			if (xmlStrcmp(componentNode->name, BAD_CAST "component"))
				continue;
			
			OERef componentRef = deviceRef.getRef(getXMLProperty(componentNode, "name"));
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				string outletStringRef = getXMLProperty(connectionNode, "ref");
				if (!outletStringRef.size())
					continue;
				
				OERef outletRef = componentRef.getRef(outletStringRef);
				if (!(outletRef.getDeviceRef() == ref.getDeviceRef()))
					continue;
				
				string stringRef = componentRef.getStringRef();
				if (!components.count(stringRef))
					return false;
				OEComponent *component = components[stringRef];
				
				setXMLProperty(connectionNode, "ref", "");
				if (!setConnection(connectionNode, component, deviceRef))
				{
					OELog("could not disconnect " + stringRef + "." +
						  getXMLProperty(connectionNode, "name"));
					
					return false;
				}
			}
		}
	}
	
	return true;
}

bool OEEmulation::insertDoc(xmlNodePtr node, xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (!node)
			return false;
		xmlAddNextSibling(node, xmlCopyNode(childNode, 1));
		node = node->next;
	}
	
	return true; 
}

//
// Helpers
//
string OEEmulation::buildSourcePath(string src, OERef deviceRef)
{
	string deviceName = deviceRef.getDevice() + "_" + deviceRef.getComponent();
	int index = src.find(OE_REF_SUBST_STRING);
	if (index != string::npos)
		src.replace(index, sizeof(OE_REF_SUBST_STRING) - 1, deviceName);
	
	return src;
}
