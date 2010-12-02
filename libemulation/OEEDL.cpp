
/**
 * libemulation
 * OEEDL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

#include <fstream>

#include "OEEDL.h"

OEEDL::OEEDL()
{
	init();
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
	if (pathExtension == OE_FILE_PATH_EXTENSION)
	{
		is_open = readFile(path, &data);
		
		if (!is_open)
			edlLog("could not open '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			is_open = package->readFile(OE_PACKAGE_EDL_PATH, &data);
			
			if (!is_open)
				edlLog("could not read '" OE_PACKAGE_EDL_PATH
					  "' in '" + path + "'");
		}
		else
			edlLog("could not open package '" + path + "'");
	}
	else
		edlLog("could not identify type of '" + path + "'");
	
	if (is_open)
	{
		doc = xmlReadMemory(&data[0],
							data.size(),
							OE_PACKAGE_EDL_PATH,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			edlLog("could not parse EDL in '" + path + "'");
		}
	}
	
	if (is_open)
	{
		is_open = validate();
		if (!is_open)
			edlLog("unknown EDL version");
	}
	
	if (!is_open)
		close();
	
	return is_open;
}

bool OEEDL::isOpen()
{
	return is_open;
}

void OEEDL::close()
{
	is_open = false;
	
	if (package)
		delete package;
	package = NULL;
}

OEHeaderInfo OEEDL::getHeaderInfo()
{
	OEHeaderInfo headerInfo;
	
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		headerInfo.label = getNodeProperty(rootNode, "label");
		headerInfo.image = getNodeProperty(rootNode, "image");
		headerInfo.description = getNodeProperty(rootNode, "description");
	}
	
	return headerInfo;
}

void OEEDL::setWindowFrame(string windowFrame)
{
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		setNodeProperty(rootNode, "windowFrame", windowFrame);
	}
}

string OEEDL::getWindowFrame()
{
	string windowFrame;
	
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		windowFrame = getNodeProperty(rootNode, "windowFrame");
	}
	
	return windowFrame;
}

OEPortsInfo OEEDL::getFreePortsInfo()
{
	OEPortsInfo freePortsInfo;
	
	if (!doc)
		return freePortsInfo;
	
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "ports"))
		{
			string ref = getNodeProperty(node, "ref");
			
			if (ref != "")
			{
				OEPortInfo portInfo;
				
				portInfo.id = getNodeProperty(node, "id");
				portInfo.type = getNodeProperty(node, "type");
				portInfo.label = getNodeProperty(node, "label");
				portInfo.image = getNodeProperty(node, "image");
				
				freePortsInfo.push_back(portInfo);
			}
		}
	
	return freePortsInfo;
}

OEConnectorsInfo OEEDL::getFreeConnectorsInfo()
{
	OEConnectorsInfo freeConnectorsInfo;
	
	if (!doc)
		return freeConnectorsInfo;

	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
		if (!xmlStrcmp(node->name, BAD_CAST "ports"))
		{
			string ref = getNodeProperty(node, "ref");
			
			if (ref != "")
			{
				OEConnectorInfo connectorInfo;
				
				connectorInfo.id = getNodeProperty(node, "id");
				connectorInfo.type = getNodeProperty(node, "type");
				connectorInfo.label = getNodeProperty(node, "label");
				connectorInfo.image = getNodeProperty(node, "image");
				
				freeConnectorsInfo.push_back(connectorInfo);
			}
		}
	
	return freeConnectorsInfo;
}

bool OEEDL::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
}

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}

bool OEEDL::hasNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	return (value != NULL);
}

string OEEDL::getNodeProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
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

string OEEDL::getPathExtension(string path)
{
	if (path.rfind(OE_PATH_SEPARATOR) == (path.length() - 1))
		path = path.substr(0, path.length() - 1);
	
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	else
		return path.substr(extensionIndex + 1);
}

void OEEDL::edlLog(string message)
{
	cerr << "libemulation: " << message << endl;
}
