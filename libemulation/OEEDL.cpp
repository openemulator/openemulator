
/**
 * libemulation
 * OEEDL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

#include <sstream>
#include <fstream>

#include "OEEDL.h"

OEEDL::OEEDL()
{
	init();
}

OEEDL::OEEDL(string path)
{
	init();
	
	open(path);
}

OEEDL::~OEEDL()
{
	close();
	
	if (doc)
		xmlFreeDoc(doc);
}

void OEEDL::init()
{
	is_open = false;
	
	package = NULL;
	doc = NULL;
}

bool OEEDL::open(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		is_open = readFile(path, &data);
		
		if (!is_open)
			OELog("could not open '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			is_open = package->readFile(OE_PACKAGE_EDL_FILENAME, &data);
			
			if (!is_open)
				OELog("could not read '" OE_PACKAGE_EDL_FILENAME
					  "', path '" + path + "'");
		}
		else
			OELog("could not open package '" + path + "'");
	}
	else
		OELog("could not identify type of '" + path + "'");
	
	if (is_open)
	{
		doc = xmlReadMemory(&data[0],
							data.size(),
							OE_PACKAGE_EDL_FILENAME,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			OELog("could not parse EDL, path '" + path + "'");
		}
	}
	
	if (is_open)
	{
		is_open = validate();
		if (!is_open)
			OELog("unknown EDL version");
	}
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEDL::isOpen()
{
	return is_open;
}

bool OEEDL::save(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		if (update())
		{
			if (dump(&data))
			{
				is_open = writeFile(path, &data);
				
				if (!is_open)
					OELog("could not open '" + path + "'");
			}
			else
				OELog("could not dump EDL, path '" + path + "'");
		}
		else
			OELog("could not update '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		
		if (package && package->isOpen())
		{
			if (update())
			{
				if (dump(&data))
				{
					is_open = package->writeFile(OE_PACKAGE_EDL_FILENAME, &data);
					if (!is_open)
						OELog("could not write '" OE_PACKAGE_EDL_FILENAME
							  "', path '" + path + "'");
				}
				else
					OELog("could not dump EDL, path '" + path + "'");
			}
			
			delete package;
			
			package = NULL;
		}
		else
			OELog("could not open '" + path + "'");
	}
	else
		OELog("could not identify type of '" + path + "'");
	
	if (!is_open)
		close();
	
	return is_open;
}

void OEEDL::close()
{
	is_open = false;
	
	if (package)
		delete package;
	package = NULL;
}

bool OEEDL::addEDL(string path, OEConnections connections)
{
	OEEDL edl(path);
	
	//	OEConnections nameMap = buildNameMap(path);
	//	if (!rename(edl, nameMap))
	//		return false;
	
	//	xmlDocPtr insertionNode = getInsertPoint(edl);
	
	//	return insert(insertionNode, edl);
	
	return true;
}

bool OEEDL::removeDevice(string id)
{
	/* Idea
	 - Determinar los dispositivos conectados a los ports del dispositivo, 
	   y borrar recursivamente
	 - Borrar todas las referencias al dispositivo
	 - Borrar todos los devices, componentes, puertos, conectores y settings con tal id
	 */
	
	/*
	xmlNodePtr deviceNode = getDeviceNode(deviceName);
	if (!deviceNode)
		return false;
	
	for (xmlNodePtr node = deviceNode->children;
		 node;
		 node++)
	{
		if (xmlStrcmp(node->name, BAD_CAST "inlet"))
			continue;
		
		string connectionRef = getNodeRef(node, deviceName);
		
		xmlNodePtr connectionNode = getConnectionNode(deviceNode, connectionRef);
		if (!connectionNode)
			return false;
		
		string componentRef = getNodeRef(connectionNode, deviceName);
		
		if (!removeDevice(getDeviceName(componentRef)))
			return false;
	}
	
	removeDevice(deviceNode);
	*/
	return true;
}

string OEEDL::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}

string OEEDL::getNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

bool OEEDL::hasNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	return (value != NULL);
}

string OEEDL::getPathExtension(string path)
{
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	return path.substr(extensionIndex + 1);
}

bool OEEDL::writeFile(string path, OEData *data)
{
	bool success = false;
	
	ofstream file(path.c_str());
	
	if (file.is_open())
	{
		file.write(&data->front(), data->size());
		success = file.good();
		file.close();
	}
	
	return success;
}

bool OEEDL::readFile(string path, OEData *data)
{
	bool success = false;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data->resize(size);
		file.read(&data->front(), size);
		success = file.good();
		file.close();
	}
	
	return success;
}

bool OEEDL::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
}

bool OEEDL::dump(OEData *data)
{
	xmlChar *p;
	int size;
	
	xmlDocDumpMemory(doc, &p, &size);
	if (!p)
		return false;
	
	data->resize(size);
	memcpy(&data->front(), p, size);
	
	return true;
}

bool OEEDL::update()
{
	return true;
}
