
/**
 * libemulator
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEComponentFactory.h"

OEEmulation::OEEmulation(string path, string resourcePath)
{
	loaded = false;
	documentDML = NULL;
	
	this->resourcePath = path;
	
	package = new OEPackage(path);
	if (package->isOpen())
	{
		vector<char> data;
		if (package->readFile(OE_INFO_FILENAME, data))
		{
			documentDML = xmlReadMemory(&data[0],
										data.size(),
										OE_INFO_FILENAME,
										NULL,
										0);
			
			if (documentDML)
			{
				if (validateDML(documentDML))
				{
					if (constructDML(documentDML))
					{
						if (initDML(documentDML))
							loaded = true;
						else
							cerr << "libemulator: could not initialize \"" + path + "\"." << endl;
						}
					else
						cerr << "libemulator: could not construct \"" + path + "\"." << endl;
				}
				else
					cerr << "libemulator: could not validate \"" + path + "\"." << endl;
			}
			else
				cerr << "libemulator: could not parse \"" + path + "\"." << endl;
		}
	}
	
	delete package;
	package = NULL;
}

OEEmulation::~OEEmulation()
{
	if (!documentDML)
		return;
	
	destroyDML(documentDML);
	xmlFreeDoc(documentDML);
}

bool OEEmulation::isLoaded()
{
	return loaded;
}

bool OEEmulation::save(string path)
{
	if (!isLoaded())
		return false;

	bool success = false;
	
	package = new OEPackage(path);
	if (package->isOpen())
	{
		if (updateDML(documentDML))
		{
			xmlChar *datap;
			int size;
			xmlDocDumpMemory(documentDML, &datap, &size);
			
			if (datap)
			{
				vector<char> data(size);
				memcpy(&data[0], datap, size);
				
				xmlFree(datap);
				
				if (package->writeFile(OE_INFO_FILENAME, data))
					success = true;
				else
					cerr << "libemulator: could not write \"" + path + "\"." << endl;
			}
			else
				cerr << "libemulator: could not dump \"" + path + "\"." << endl;
		}
		else
			cerr << "libemulator: could not update \"" + path + "\"." << endl;
		
		if (!success)
			package->remove();
	}
	else
		cerr << "libemulator: could not open \"" + path + "\"." << endl;
	
	delete package;
	package = NULL;
	
	return success;
}

int OEEmulation::ioctl(string ref, int message, void *data)
{
	if (!isLoaded())
		return 0;
	
	if (!components.count(ref))
		return 0;
	OEComponent *component = components[ref];
	
	return component->ioctl(message, data);
}

xmlDocPtr OEEmulation::getDML()
{
	return documentDML;
}

bool OEEmulation::addDevices(string path, OEStringRefMap connections)
{
	if (!isLoaded())
		return false;
	
	vector<char> data;
	if (!readFile(path, data))
		return false;
	
	xmlDocPtr doc;
	doc = xmlReadMemory(&data[0],
						data.size(),
						OE_INFO_FILENAME,
						NULL,
						0);
	if (!doc)
		return false;
	
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
						cerr << "libemulator: could not connect \"" + path + "\"." << endl;
				}
				else
					cerr << "libemulator: could not insert \"" + path + "\"." << endl;
			}
			else
				cerr << "libemulator: could not initialize \"" + path + "\"." << endl;
		}
		else
			cerr << "libemulator: could not construct \"" + path + "\"." << endl;
	}
	else
		cerr << "libemulator: could not validate \"" + path + "\"." << endl;
	
	xmlFreeDoc(doc);
	
	return success;
}

bool OEEmulation::isDeviceTerminal(OERef ref)
{
	if (!isLoaded())
		return false;
	
	return (getNodeOfFirstInlet(documentDML, ref) == NULL);
}

bool OEEmulation::removeDevice(OERef ref)
{
	if (!isLoaded())
		return false;
	
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(documentDML, deviceRef);
	if (!deviceNode)
	{
		cerr << "libemulator: could not remove \"" + ref.getStringRef() + "\"." << endl;
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
		if (!removeDevice(outletRef))
			return false;
	}
	
	disconnectDevice(documentDML, deviceRef);
	destroyDevice(deviceNode);
	
	return true;
}

string OEEmulation::toString(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

bool OEEmulation::readFile(string path, vector<char> &data)
{
	bool success = false;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data.resize(size);
		file.read(&data[0], size);
		success = file.good();
		file.close();
	}
	
	return success;
}

string OEEmulation::buildSourcePath(string src, OERef deviceRef)
{
	string deviceName = deviceRef.getStringRef();
	int index = src.find(OE_DEVICENAME_SUBST_STRING);
	if (index != string::npos)
		src.replace(index, sizeof(OE_DEVICENAME_SUBST_STRING) - 1, deviceName);
	
	return src;
}

string OEEmulation::getXMLProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEEmulation::setXMLProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}

bool OEEmulation::validateDML(xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(rootNode, "version") == "1.0");
}

bool OEEmulation::constructDML(xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!constructDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::initDML(xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!initDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::updateDML(xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		updateDevice(childNode);
	}
	
	return true;
}

void OEEmulation::destroyDML(xmlDocPtr doc)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		destroyDevice(childNode);
	}
}

bool OEEmulation::constructDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!constructComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::initDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!initComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

bool OEEmulation::updateDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!updateComponent(childNode, deviceRef))
			return false;
	}
	
	return true;
}

void OEEmulation::destroyDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		destroyComponent(childNode, deviceRef);
	}
}

bool OEEmulation::constructComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentClass = getXMLProperty(node, "class");
	string componentName = getXMLProperty(node, "name");
	OEComponent *component = OEComponentFactory::build(string(componentClass));
	if (!component)
		return false;
	
	string componentRef = deviceRef.getStringRef(componentName);
//	printf("OEEmulation::constructComponent: %s\n", componentRef.c_str());
	
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
		return false;
	OEComponent *component = components[componentRef];
	
	//	printf("OEEmulation::initComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "connection"))
		{
			if (!setConnection(childNode, component, deviceRef))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!setProperty(childNode, component))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
		{
			if (!setData(childNode, component, deviceRef))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "resource"))
		{
			if (!setResource(childNode, component))
				return false;
		}
	}
	
	return true;
}

bool OEEmulation::updateComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	if (!components.count(componentRef))
		return false;
	OEComponent *component = components[componentRef];
	
	//	printf("OEEmulation::queryComponent: %s\n", componentRef.c_str());
	
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!xmlStrcmp(childNode->name, BAD_CAST "property"))
		{
			if (!getProperty(childNode, component))
				return false;
		}
		else if (!xmlStrcmp(childNode->name, BAD_CAST "data"))
		{
			if (!getData(childNode, component, deviceRef))
				return false;
		}
	}
	
	return true;
}

void OEEmulation::destroyComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
	
	if (!components.count(componentRef))
		return;
	components.erase(componentRef);
}

bool OEEmulation::setConnection(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	OEComponent *connectedComponent;
	
	string name = getXMLProperty(node, "name");
	string ref = getXMLProperty(node, "ref");
	
	if (!ref.size())
		connectedComponent = NULL;
	else
	{
		string componentRef = deviceRef.getStringRef(ref);
		if (!components.count(componentRef))
		{
			cerr << "libemulator: could not connect \"" + componentRef + "\"." << endl;
			return false;
		}
		connectedComponent = components[componentRef];
	}
	
	OEIoctlConnection msg;
	msg.name = name;
	msg.component = connectedComponent;
	
	component->ioctl(OEIoctlSetConnection, &msg);
	
	return true;
}

bool OEEmulation::setProperty(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	string value = getXMLProperty(node, "value");
	
	OEIoctlProperty msg;
	msg.name = name;
	msg.value = value;
	
	component->ioctl(OEIoctlSetProperty, &msg);
	
	return true;
}

bool OEEmulation::getProperty(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	
	OEIoctlProperty msg;
	msg.name = name;
	
	if (component->ioctl(OEIoctlGetProperty, &msg))
		setXMLProperty(node, "value", msg.value);
	
	return true;
}

bool OEEmulation::setData(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), deviceRef);
	
	OEIoctlData msg;
	msg.name = name;
	
	if (package && package->readFile(src, msg.data))
		component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

bool OEEmulation::getData(xmlNodePtr node, OEComponent *component, OERef deviceRef)
{
	string name = getXMLProperty(node, "name");
	string src = buildSourcePath(getXMLProperty(node, "src"), deviceRef);
	
	OEIoctlData msg;
	msg.name = name;
	
	if (component->ioctl(OEIoctlGetData, &msg))
	{
		if (!package->writeFile(src, msg.data))
		{
			cerr << "libemulator: could not write \"" + src + "\"." << endl;
			return false;
		}
	}
	
	return true;
}

bool OEEmulation::setResource(xmlNodePtr node, OEComponent *component)
{
	string name = getXMLProperty(node, "name");
	string src = resourcePath + OE_PATH_SEPARATOR + getXMLProperty(node, "src");
	
	OEIoctlData msg;
	msg.name = name;
	
	if (!readFile(src, msg.data))
	{
		cerr << "libemulator: could not read \"" + src + "\"." << endl;
		return false;
	}
	
	component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

xmlNodePtr OEEmulation::getNodeForRef(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	if (!ref.getDevice().size())
		return NULL;
	
	for(xmlNodePtr deviceNode = rootNode->children;
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

OERef OEEmulation::getOutletForInlet(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr connectionNode = getNodeForRef(doc, ref);
	if (!connectionNode)
		return OERef();
	
	string outletStringRef = getXMLProperty(connectionNode, "ref");
	if (outletStringRef.size())
		return ref.getRef(outletStringRef);
	else
		return OERef();
}

xmlNodePtr OEEmulation::getNodeOfFirstInlet(xmlDocPtr, OERef ref)
{
	OERef deviceRef = ref.getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(documentDML, deviceRef);
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
			newDeviceName = "dev" + toString(devIndex);
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
		
		string componentRef = inletRef.getComponentRef().getStringRef();
		if (!components.count(componentRef))
			return false;
		OEComponent *component = components[componentRef];
		
		setXMLProperty(node, "ref", i->second);
		setConnection(node, component, deviceRef);
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
			
			string componentName = getXMLProperty(componentNode, "name");
			string componentRef = deviceRef.getStringRef(componentName);
			
			for(xmlNodePtr connectionNode = componentNode->children;
				connectionNode;
				connectionNode = connectionNode->next)
			{
				if (xmlStrcmp(connectionNode->name, BAD_CAST "connection"))
					continue;
				
				OERef inletRef = deviceRef.getRef(getXMLProperty(connectionNode, "ref"));
				if (!(inletRef == ref))
					continue;
				
				if (!components.count(componentRef))
					return false;
				OEComponent *component = components[componentRef];
				
				setXMLProperty(connectionNode, "ref", "");
				setConnection(connectionNode, component, deviceRef);
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
