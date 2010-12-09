
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
	is_open = false;
	
	package = NULL;
	doc = NULL;
}

OEEDL::~OEEDL()
{
	close();
	
	if (doc)
		xmlFreeDoc(doc);
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
		package = new OEPackage();
		if (package && package->open(path))
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
	
/*	if (is_open)
	{
		xmlValidCtxtPtr validCtxt = xmlNewValidCtxt();
		xmlDtdPtr dtd = xmlNewDtd(doc,
								  <#const xmlChar *name#>, <#const xmlChar *ExternalID#>, <#const xmlChar *SystemID#>)
		
		if (!xmlValidateDocumentFinal(validCtxt, doc))
		{
			is_open = false;
			edlLog("could not validate DTD of EDL in '" + path + "'");
		}
		
		xmlFreeValidCtxt(validCtxt);
	}*/
	
	if (is_open && !validateEmulation())
	{
		is_open = false;
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

string OEEDL::getOptions()
{
	string value;
	
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		value = getNodeProperty(rootNode, "windowFrame");
	}
	
	return value;
}

void OEEDL::setOptions(string value)
{
	if (doc)
	{
		xmlNodePtr rootNode = xmlDocGetRootElement(doc);
		setNodeProperty(rootNode, "windowFrame", value);
	}
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

bool OEEDL::validateEmulation()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
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

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
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

string OEEDL::getPathExtension(string path)
{
	// Remove tailing path separator
	if (path.rfind(OE_PATH_SEPARATOR) == (path.length() - 1))
		path = path.substr(0, path.length() - 1);
	
	// Find extension
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	// Convert to lower case
	for (int i = 0; i < path.size(); i++)
		path[i] = tolower(path[i]);
	
	return path.substr(extensionIndex + 1);
}

void OEEDL::edlLog(string message)
{
	cerr << "libemulation: " << message << endl;
}
