
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

OEEmulation::OEEmulation(string emulationPath, string resourcePath)
{
	documentDML = NULL;
	open = false;
	
	this->resourcePath = resourcePath;
	package = new OEPackage(emulationPath);
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
			
			constructDML(documentDML);
			if (initDML(documentDML))
				open = true;
		}
	}
	
	delete package;
	package = NULL;
}

OEEmulation::~OEEmulation()
{
	destroyDML(documentDML);
	
	if (documentDML)
		xmlFreeDoc(documentDML);
}

bool OEEmulation::isOpen()
{
	return open;
}

bool OEEmulation::save(string emulationPath)
{
	bool error = true;
	
	package = new OEPackage(emulationPath);
	if (package->isOpen())
	{
		if (open && updateDML(documentDML))
		{
			xmlChar *datap;
			int size;
			xmlDocDumpMemory(documentDML, &datap, &size);
			
			if (datap)
			{
				vector<char> data(size);
				memcpy(&data[0], datap, size);
				
				xmlFree(datap);
				
				error = !package->writeFile(OE_INFO_FILENAME, data);
			}
		}
		
		if (error)
			package->remove();
	}
	
	return !error;
}

int OEEmulation::ioctl(string ref, int message, void *data)
{
	if (open)
	{
		OEComponent *component = components[ref];
		
		if (component)
			return component->ioctl(message, data);
	}
	
	return 0;
}

xmlDocPtr OEEmulation::getDML()
{
	return documentDML;
}

bool OEEmulation::addDevices(string path, OEStringRefMap connections)
{
	vector<char> data;
	if (!readFile(path, data))
		return false;
	xmlDocPtr deviceDML;
	deviceDML = xmlReadMemory(&data[0],
							  data.size(),
							  OE_INFO_FILENAME,
							  NULL,
							  0);
	if (!deviceDML)
		return false;
	if (!validateDML(deviceDML))
	{
		xmlFreeDoc(deviceDML);
		return false;
	}
	
	map<string, string> deviceNameMap;
	buildDeviceNameMap(documentDML, deviceDML, deviceNameMap);
	renameDMLRefs(deviceDML, deviceNameMap);
	renameConnections(connections, deviceNameMap);
	
	if (mergeDMLs(documentDML, deviceDML, connections))
	{
		constructDML(deviceDML);
		reconnectDML(documentDML);
	}
	
	xmlFreeDoc(deviceDML);
	
	return true;
}

bool OEEmulation::isDeviceAtInletTerminal(OERef ref)
{
	OERef deviceRef = getOutletForInlet(documentDML, ref).getDeviceRef();
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
		if (!outletRef.isEmpty())
			return false;
	}
	
	return true;
}

bool OEEmulation::removeDevicesOnInlet(OERef ref)
{
	OERef deviceRef = getOutletForInlet(documentDML, ref).getDeviceRef();
	xmlNodePtr deviceNode = getNodeForRef(documentDML, deviceRef);
	if (!deviceNode)
		return false;
	
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		if (!removeDevicesOnInlet(inletRef))
			return false;
		setXMLProperty(inletNode, "ref", "");
	}
	
	destroyComponent(deviceNode, deviceRef);
	
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
	bool error = true;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data.resize(size);
		file.read(&data[0], size);
		error = !file.good();
		file.close();
	}
	
	return !error;
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

string OEEmulation::buildSourcePath(string src, OERef deviceRef)
{
	string deviceName = deviceRef.getStringRef();
	int index = src.find(OE_DEVICENAME_SUBST_STRING);
	if (index != string::npos)
		src.replace(index, sizeof(OE_DEVICENAME_SUBST_STRING) - 1, deviceName);
	
	return src;
}

bool OEEmulation::readResource(string localPath, vector<char> &data)
{
	bool error = true;
	
	ifstream file((resourcePath + OE_PATH_SEPARATOR + localPath).c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data.resize(size);
		file.read(&data[0], size);
		error = !file.good();
		file.close();
	}
	
	return !error;
}

bool OEEmulation::validateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	return (getXMLProperty(node, "version") == "1.0");
}

bool OEEmulation::constructDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
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
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
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

bool OEEmulation::reconnectDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "device"))
			continue;
		
		if (!reconnectDevice(childNode))
			return false;
	}
	
	return true;
}

bool OEEmulation::updateDML(xmlDocPtr doc)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
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
	xmlNodePtr node = xmlDocGetRootElement(doc);
	for(xmlNodePtr childNode = node->children;
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

bool OEEmulation::reconnectDevice(xmlNodePtr node)
{
	OERef deviceRef(getXMLProperty(node, "name"));
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (xmlStrcmp(childNode->name, BAD_CAST "component"))
			continue;
		
		if (!reconnectComponent(childNode, deviceRef))
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
	
	if (component)
	{
		string componentRef = deviceRef.getStringRef(componentName);
//		printf("OEEmulation::constructComponent: %s\n", componentRef.c_str());
		
		if (components[componentRef])
			delete components[componentRef];
		
		components[componentRef] = component;
	}
	
	return (component != NULL);
}

bool OEEmulation::initComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
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

bool OEEmulation::reconnectComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
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
	}
	
	return true;
}

bool OEEmulation::updateComponent(xmlNodePtr node, OERef deviceRef)
{
	string componentName = getXMLProperty(node, "name");
	string componentRef = deviceRef.getStringRef(componentName);
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
	
	delete components[componentRef];
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
		connectedComponent = components[componentRef];
		
		if(!connectedComponent)
		{
			cerr << "libemulator: could not connect \"" + componentRef + "\"." << endl;
			return false;
		}
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
	
	if (package->readFile(src, msg.data))
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
	string src = getXMLProperty(node, "src");
	
	OEIoctlData msg;
	msg.name = name;
	
	if (!readResource(src, msg.data))
	{
		cerr << "libemulator: could not read \"" + src + "\"." << endl;
		return false;
	}
	
	component->ioctl(OEIoctlSetData, &msg);
	
	return true;
}

xmlNodePtr OEEmulation::getNodeForRef(xmlDocPtr doc, OERef ref)
{
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
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

void OEEmulation::buildDeviceNameMap(xmlDocPtr doc, xmlDocPtr elem,
									 OEStringRefMap &deviceNameMap)
{
	xmlNodePtr node = xmlDocGetRootElement(elem);
	
	int devIndex = 0;
	for(xmlNodePtr childNode = node->children;
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

void OEEmulation::renameDMLRefs(xmlDocPtr doc, OEStringRefMap &deviceNameMap)
{
	xmlNodePtr node = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr deviceNode = node->children;
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
}

void OEEmulation::renameConnections(OEStringRefMap &connections,
									OEStringRefMap &deviceNameMap)
{
	for (OEStringRefMap::iterator i = connections.begin();
		 i != connections.end();
		 i++)
	{
		OERef outletRef = OERef(i->second);
		outletRef.setDevice(deviceNameMap[outletRef.getDevice()]);
		connections[i->first] = outletRef.getStringRef();
	}
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
		return NULL;
	
	OERef prevRef = deviceRef;
	vector<OERef> visitedRefs;
	for(xmlNodePtr inletNode = deviceNode->children;
		inletNode;
		inletNode = inletNode->next)
	{
		if (xmlStrcmp(inletNode->name, BAD_CAST "inlet"))
			continue;
		
		OERef inletRef = deviceRef.getRef(getXMLProperty(inletNode, "ref"));
		if (inletRef == ref)
			return getNodeOfLastInlet(doc, prevRef, visitedRefs);
		
		OERef outletRef = getOutletForInlet(doc, inletRef);
		if (!outletRef.isEmpty())
			prevRef = outletRef;
	}
	
	return deviceNode;
}

bool OEEmulation::mergeDMLs(xmlDocPtr doc, xmlDocPtr elem, OEStringRefMap &connections)
{
	OERef firstInletRef;
	OEStringRefMap::iterator i = connections.begin();
	if (i != connections.end())
		firstInletRef = OERef(i->first);
	
	xmlNodePtr previousNode = getNodeOfPreviousInlet(doc, firstInletRef);
	xmlNodePtr node = xmlCopyNode(xmlDocGetRootElement(elem), 1);
	for(xmlNodePtr childNode = node->children;
		childNode;
		childNode = childNode->next)
	{
		if (!previousNode)
			return false;
		xmlAddNextSibling(previousNode, xmlCopyNode(childNode, 1));
		previousNode = previousNode->next;
	}
	
	for (OEStringRefMap::iterator i = connections.begin();
		 i != connections.end();
		 i++)
	{
		xmlNodePtr node = getNodeForRef(doc, OERef(i->first));
		if (node)
			setXMLProperty(node, "ref", i->second);
	}
	
	return true;
}
