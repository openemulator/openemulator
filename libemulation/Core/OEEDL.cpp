
/**
 * libemulation
 * OEEDL
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

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
			logMessage("could not open '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage();
		if (package && package->open(path))
		{
			is_open = package->readFile(OE_PACKAGE_EDL_PATH, &data);
			
			if (!is_open)
				logMessage("could not read '" OE_PACKAGE_EDL_PATH
                           "' in '" + path + "'");
		}
		else
			logMessage("could not open package '" + path + "'");
	}
	else
		logMessage("could not identify type of '" + path + "'");
	
	if (is_open)
	{
		doc = xmlReadMemory(&data[0],
							(int) data.size(),
							OE_PACKAGE_EDL_PATH,
							NULL,
							0);
		
		if (!doc)
		{
			is_open = false;
			logMessage("could not parse EDL in '" + path + "'");
		}
	}
	
    /* To-Do: validate DTD
     if (is_open)
     {
     xmlValidCtxtPtr validCtxt = xmlNewValidCtxt();
     xmlDtdPtr dtd = xmlNewDtd(doc,
     name,
     ExternalID,
     SystemID)
     
     if (!xmlValidateDocumentFinal(validCtxt, doc))
     {
     is_open = false;
     logMessage("could not validate DTD of EDL in '" + path + "'");
     }
     
     xmlFreeValidCtxt(validCtxt);
     }*/
	
	if (is_open && !validateEmulation())
	{
		is_open = false;
		logMessage("unknown EDL version");
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
	if (pathExtension == OE_FILE_PATH_EXTENSION)
	{
		if (updateEmulation())
		{
			if (dumpEmulation(&data))
			{
				is_open = writeFile(path, &data);
				
				if (!is_open)
					logMessage("could not open '" + path + "'");
			}
			else
				logMessage("could not dump EDL for '" + path + "'");
		}
		else
			logMessage("could not update the configuration for '" + path + "'");
	}
	else if (pathExtension == OE_PACKAGE_PATH_EXTENSION)
	{
		package = new OEPackage();
		if (package && package->open(path))
		{
			if (updateEmulation())
			{
				if (dumpEmulation(&data))
				{
					is_open = package->writeFile(OE_PACKAGE_EDL_PATH, &data);
					if (!is_open)
						logMessage("could not write '" OE_PACKAGE_EDL_PATH
								   "' in '" + path + "'");
				}
				else
					logMessage("could not dump EDL for '" + path + "'");
			}
            else
                logMessage("could not update the configuration for '" + path + "'");
			
			delete package;
			package = NULL;
		}
		else
			logMessage("could not open '" + path + "'");
	}
	else
		logMessage("could not identify type of '" + path + "'");
	
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

OEPortsInfo OEEDL::getFreePortsInfo()
{
	OEPortsInfo freePortsInfo;
	
	if (doc)
    {
        xmlNodePtr rootNode = xmlDocGetRootElement(doc);
        
        for(xmlNodePtr node = rootNode->children;
            node;
            node = node->next)
            if (!xmlStrcmp(node->name, BAD_CAST "port"))
            {
                string ref = getNodeProperty(node, "ref");
                
                if (ref == "")
                {
                    OEPortInfo portInfo;
                    portInfo.id = getNodeProperty(node, "id");
                    portInfo.type = getNodeProperty(node, "type");
                    portInfo.label = getNodeProperty(node, "label");
                    portInfo.image = getNodeProperty(node, "image");
                    
                    freePortsInfo.push_back(portInfo);
                }
            }
    }
    
	return freePortsInfo;
}

OEConnectorsInfo OEEDL::getFreeConnectorsInfo()
{
	OEConnectorsInfo freeConnectorsInfo;
	
	if (doc)
    {
        xmlNodePtr rootNode = xmlDocGetRootElement(doc);
        
        for(xmlNodePtr node = rootNode->children;
            node;
            node = node->next)
            if (!xmlStrcmp(node->name, BAD_CAST "connector"))
            {
                string ref = getNodeProperty(node, "ref");
                
                if (ref == "")
                {
                    OEConnectorInfo connectorInfo;
                    
                    connectorInfo.id = getNodeProperty(node, "id");
                    connectorInfo.type = getNodeProperty(node, "type");
                    connectorInfo.label = getNodeProperty(node, "label");
                    connectorInfo.image = getNodeProperty(node, "image");
                    
                    freeConnectorsInfo.push_back(connectorInfo);
                }
            }
    }
	
	return freeConnectorsInfo;
}

bool OEEDL::addEDL(string path, OEIdMap connections)
{
	if (!doc)
		return false;
	
	OEEDL edl;
	if (!edl.open(path))
		return false;
	
    // Remap id names
    OEIds deviceIds = edl.getDeviceIds();
	OEIdMap deviceIdMap = makeIdMap(deviceIds);
    
    edl.remapDocument(deviceIdMap);
	remapConnections(connections, deviceIdMap);
    
    // Insert EDL
    string firstPortId = connections.begin()->first;
    xmlNodePtr node = getInsertionNode(firstPortId);
    if (!node)
    {
        logMessage("could not find insertion node for '" + path + "'");
        
        return false;
    }
    
    if (!edl.insertInto(node))
    {
        logMessage("could not insert '" + path + "'");
        
        return false;
    }
    
    // To-Do: Connect
    
    return true;
}

OEIds OEEDL::getDeviceIds()
{
    OEIds deviceIds;
    
	if (doc)
    {
        xmlNodePtr rootNode = xmlDocGetRootElement(doc);
        
        for(xmlNodePtr node = rootNode->children;
            node;
            node = node->next)
            if (!xmlStrcmp(node->name, BAD_CAST "device"))
            {
                string deviceId = getNodeProperty(node, "id");
                
                deviceIds.push_back(deviceId);
            }
    }
    
	return deviceIds;
}

void OEEDL::setDeviceId(string& id, string deviceId)
{
	int dotIndex = (int) id.find_first_of('.');
	
	if (dotIndex == string::npos)
		id = deviceId;
	else
		id = deviceId + "." + id.substr(dotIndex + 1);
}

string OEEDL::getDeviceId(string id)
{
	int dotIndex = (int) id.find_first_of('.');
	
	if (dotIndex == string::npos)
		return id;
	
	return id.substr(0, dotIndex);
}




bool OEEDL::validateEmulation()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	return (getNodeProperty(rootNode, "version") == "1.0");
}

bool OEEDL::dumpEmulation(OEData *data)
{
	if (!doc)
		return false;
	
	xmlChar *xmlDump;
	int xmlDumpSize;
	
	xmlDocDumpMemory(doc, &xmlDump, &xmlDumpSize);
	if (xmlDump)
	{
		data->resize(xmlDumpSize);
		memcpy(&data->front(), xmlDump, xmlDumpSize);
		
		xmlFree(xmlDump);
		
		return true;
	}
	
	return false;
}

bool OEEDL::updateEmulation()
{
    return true;
}



// Make an id map so a new document can be inserted to the current document
OEIdMap OEEDL::makeIdMap(OEIds& deviceIds)
{
    OEIds ourDeviceIds = getDeviceIds();
    
	OEIdMap idMap;
	
	for (OEIds::iterator i = deviceIds.begin();
		 i != deviceIds.end();
		 i++)
	{
		string deviceId = *i;
        
		int newDeviceIdCount = 1;
		string newDeviceId = deviceId;
		while (find(ourDeviceIds.begin(), ourDeviceIds.end(), newDeviceId) !=
               ourDeviceIds.end())
		{
			newDeviceIdCount++;
			newDeviceId = deviceId + getString(newDeviceIdCount);
		}
		
		idMap[deviceId] = newDeviceId;
	}
	
	return idMap;
}

// Rename document ids
void OEEDL::remapNode(OEIdMap& deviceIdMap, xmlNodePtr node, string property)
{
	string nodeProperty;
	
	if (hasNodeProperty(node, property))
	{
		string id = getNodeProperty(node, property);
		string deviceId = getDeviceId(id);
        
		if (deviceIdMap.count(deviceId))
		{
			setDeviceId(id, deviceIdMap[deviceId]);
			setNodeProperty(node, property, id);
		}
	}
}

// Rename document ids
void OEEDL::remapDocument(OEIdMap& deviceIdMap)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		remapNode(deviceIdMap, node, "id");
		remapNode(deviceIdMap, node, "ref");
		
		if (node->children)
		{
			for(xmlNodePtr propertyNode = node->children;
				propertyNode;
				propertyNode = propertyNode->next)
			{
				remapNode(deviceIdMap, propertyNode, "id");
				remapNode(deviceIdMap, propertyNode, "ref");
			}
		}
	}
}

// Rename connections ids
void OEEDL::remapConnections(OEIdMap& connections, OEIdMap& deviceIdMap)
{
	for (OEIdMap::iterator i = connections.begin();
		 i != connections.end();
		 i++)
	{
		string srcId = i->first;
		string destId = i->second;
		
		string deviceId = getDeviceId(srcId);
        if (deviceIdMap.count(deviceId))
        {
            setDeviceId(destId, deviceIdMap[deviceId]);
            connections[srcId] = destId;
        }
    }
}

// Find last node belonging to a device
xmlNodePtr OEEDL::getLastNode(string deviceId)
{
    xmlNodePtr lastNode = NULL;
    xmlNodePtr lastDocNode = NULL;
    
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        string id = getNodeProperty(node, "id");
        
        if (getDeviceId(id) == deviceId)
            lastNode = node;
        
        lastDocNode = node;
    }
    
    if (!lastNode)
        lastNode = lastDocNode;
    
    return lastNode;
}

// Follow connection chain
string OEEDL::followChain(string deviceId, vector<string>& visitedIds)
{
	// Check circularity
	if (find(visitedIds.begin(), visitedIds.end(), deviceId) != visitedIds.end())
		return deviceId;
	visitedIds.push_back(deviceId);
	
    string lastDeviceId = deviceId;
    
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
		if (!xmlStrcmp(node->name, BAD_CAST "port"))
		{
			string portId = getNodeProperty(node, "id");
			string ref = getNodeProperty(node, "ref");
			
			if (getDeviceId(portId) == deviceId)
				lastDeviceId = followChain(getDeviceId(ref), visitedIds);
		}
    }
    
    return lastDeviceId;
}

// Find node for inserting another document
xmlNodePtr OEEDL::getInsertionNode(string portId)
{
    string portDeviceId = getDeviceId(portId);
    
    // Find the previously used port
    string insertDeviceId;
    
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        string id = getNodeProperty(node, "id");
        
        if (getDeviceId(id) == portDeviceId)
        {
            if (!xmlStrcmp(node->name, BAD_CAST "port"))
            {
                if (id == portId)
                    break;
                
                string ref = getNodeProperty(node, "ref");
                
                // Follow the connection chain to the last device
                if (ref != "")
                {
                    vector<string> visitedIds;
                    insertDeviceId = followChain(getDeviceId(ref), visitedIds);
                }
            }
        }
    }
    
    if (insertDeviceId == "")
        insertDeviceId = portDeviceId;
    
    // Return last node of the insertion point
    return getLastNode(insertDeviceId);
}

// Insert this document into dest
bool OEEDL::insertInto(xmlNodePtr dest)
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr childNode = rootNode->children;
		childNode;
		childNode = childNode->next)
	{
		if (!dest)
			return false;
        
        // Don't add last character data
        if ((childNode->type != XML_TEXT_NODE) ||
            childNode->next)
            xmlAddNextSibling(dest, xmlCopyNode(childNode, 1));
        
        dest = dest->next;
	}
    
    return true;
}

string OEEDL::getLocationLabel(string deviceId, vector<string>& visitedIds)
{
	if (!doc)
		return "";
    
	// Check circularity
	if (find(visitedIds.begin(), visitedIds.end(), deviceId) != visitedIds.end())
		return "*";
	visitedIds.push_back(deviceId);
	
	// Follow connection chain
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "port"))
		{
			string portId = getNodeProperty(node, "id");
			string ref = getNodeProperty(node, "ref");
			string label = getNodeProperty(node, "label");
			
			if (getDeviceId(ref) == deviceId)
				return (getLocationLabel(getDeviceId(portId), visitedIds) +
						" " + label);
		}
	}
	
	// Find device
	for(xmlNodePtr node = rootNode->children;
		node;
		node = node->next)
	{
		if (!xmlStrcmp(node->name, BAD_CAST "device"))
		{
			string theDeviceId = getNodeProperty(node, "id");
			string label = getNodeProperty(node, "label");
			
			if (theDeviceId == deviceId)
				return label;
		}
	}
	
	// Device was not found
	return "?";
}

string OEEDL::getLocationLabel(string id)
{
	if (!doc)
		return "";
	
	vector<string> visitedIds;
	string location = getLocationLabel(id, visitedIds);
	int depth = (int) visitedIds.size();
	
	if (depth == 1)
		return "";
	else
		return location;
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
	xmlFree(value);
	
	return (value != NULL);
}

void OEEDL::setNodeProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}
