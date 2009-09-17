
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <stdio.h>
#include <dirent.h>
#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "zip.h"

#include "emulator.h"
#include "Component.h"

#define DMLINFO_FILENAME "info.xml"

using namespace std;

class EmulationClass
{
private:
	xmlDocPtr dml;
	vector<Component *> components;
};

DMLPortNode *dmlPortParse(string deviceName, xmlNodePtr node)
{
	DMLPortNode *portNode = new DMLPortNode;
	
	if (!portNode)
		return NULL;
	
	portNode->ref = "";
	portNode->type = "";
	portNode->subtype = "";
	portNode->label = "";
	portNode->image = "";
	portNode->next = NULL;
	xmlChar *value;
	
	if ((value = xmlGetProp(node, BAD_CAST "ref")) != NULL)
	{
		string ref = (char *) value;
		if (ref.find("::") == string::npos)
			ref = deviceName + "::" + ref;
		portNode->ref = (char *) xmlCharStrdup(ref.c_str());
	}
	if ((value = xmlGetProp(node, BAD_CAST "type")) != NULL)
		portNode->type = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(node, BAD_CAST "subtype")) != NULL)
		portNode->subtype = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(node, BAD_CAST "label")) != NULL)
		portNode->label = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(node, BAD_CAST "image")) != NULL)
		portNode->image = (char *) xmlStrdup(value);
	
	return portNode;
}

void dmlPortFree(DMLPortNode *node)
{
	xmlFree(node->ref);
	xmlFree(node->type);
	xmlFree(node->subtype);
	xmlFree(node->label);
	xmlFree(node->image);
	
	delete node;
}

DMLInfo *dmlInfoParse(xmlDocPtr doc)
{
	DMLInfo *dmlInfo = new DMLInfo;

	if (!dmlInfo)
		return NULL;
	
	dmlInfo->label = "";
	dmlInfo->image = "";
	dmlInfo->description = "";
	dmlInfo->group = "";
	dmlInfo->inlets = NULL;
	dmlInfo->outlets = NULL;
	
	xmlNodePtr dmlNode = xmlDocGetRootElement(doc);
	
	xmlChar *value;
	if ((value = xmlGetProp(dmlNode, BAD_CAST "label")) != NULL)
		dmlInfo->label = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(dmlNode, BAD_CAST "image")) != NULL)
		dmlInfo->image = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(dmlNode, BAD_CAST "description")) != NULL)
		dmlInfo->description = (char *) xmlStrdup(value);
	if ((value = xmlGetProp(dmlNode, BAD_CAST "group")) != NULL)
		dmlInfo->group = (char *) xmlStrdup(value);
	
	for(xmlNodePtr deviceNode = dmlNode->children;
		deviceNode;
		deviceNode = deviceNode->next)
	{
		string deviceName = (char *) deviceNode->name;
		DMLPortNode **nextInlet = &dmlInfo->inlets;
		DMLPortNode **nextOutlet = &dmlInfo->inlets;
		
		for(xmlNodePtr childNode = deviceNode->children;
			childNode;
			childNode = childNode->next)
		{
			if (!xmlStrcmp(childNode->name, BAD_CAST "inlet"))
			{
				*nextInlet = dmlPortParse(deviceName, childNode);
				nextInlet = &(*nextInlet)->next;
			}
			if (!xmlStrcmp(childNode->name, BAD_CAST "outlet"))
			{
				*nextOutlet = dmlPortParse(deviceName, childNode);
				nextOutlet = &(*nextOutlet)->next;
			}
		}
	}
	
	return dmlInfo;
}

extern "C" DMLInfo *dmlInfoRead(char *path)
{
	DMLInfo *dmlInfo;
	
	xmlDocPtr doc = xmlReadFile(path, NULL, 0);
	if (doc)
		dmlInfo = dmlInfoParse(doc);
	xmlFreeDoc(doc);
	
	return dmlInfo;
}

extern "C" DMLInfo *dmlInfoReadFromTemplate(char *path)
{
	DMLInfo *dmlInfo = NULL;
	
	struct zip *zipFile;
	if ((zipFile = zip_open(path, 0, NULL)) != NULL)
	{
		struct zip_stat dmlFileStat;
		if (zip_stat(zipFile, DMLINFO_FILENAME, 0, &dmlFileStat) == 0)
		{
			int dmlFileSize = dmlFileStat.size;
			struct zip_file *dmlFile;
			
			if ((dmlFile = zip_fopen(zipFile, DMLINFO_FILENAME, 0)) != NULL)
			{
				char dmlData[dmlFileSize];
				if (zip_fread(dmlFile, dmlData, dmlFileSize) == dmlFileSize)
				{
					xmlDocPtr doc = xmlReadMemory(dmlData,
												  dmlFileSize,
												  DMLINFO_FILENAME,
												  NULL,
												  0);
					if (doc)
						dmlInfo = dmlInfoParse(doc);
					xmlFreeDoc(doc);
				}
				
				zip_fclose(dmlFile);
			}
		}
		
		zip_close(zipFile);
	}
	
	return dmlInfo;
}

extern "C" void dmlInfoFree(DMLInfo *dmlInfo)
{
	DMLPortNode *node;
	
	node = dmlInfo->inlets;
	while(node)
	{
		DMLPortNode *next = node->next;
		dmlPortFree(node);
		node = next;
	}
	node = dmlInfo->outlets;
	while(node)
	{
		DMLPortNode *next = node->next;
		dmlPortFree(node);
		node = next;
	}
	
	xmlFree(dmlInfo->label);
	xmlFree(dmlInfo->description);
	xmlFree(dmlInfo->image);
	xmlFree(dmlInfo->group);
	
	delete dmlInfo;
}

extern "C" DMLConnections *dmlConnectionNew()
{
	return new DMLConnections;
}

extern "C" int dmlConnectionsAdd(DMLConnections *conn, char *inletRef, char *outletRef)
{
	DMLConnectionNode *node = new DMLConnectionNode;
	node->next = *conn;
	*conn = node;
	
	node->inletRef = (char *) xmlCharStrdup(inletRef);
	node->outletRef = (char *) xmlCharStrdup(outletRef);
	
	return 0;
}

extern "C" void dmlConnectionsFree(DMLConnections * conn)
{
	DMLConnectionNode *node = *conn;
	
	while (node)
	{
		DMLConnectionNode *next = node->next;
		
		free(node->inletRef);
		free(node->outletRef);
		delete node;
		
		node = next;
	}
	
	delete conn;
}

extern "C" Emulation *emulatorOpen(char * path)
{
	// Read DML
	// Instantiate all components
	// Send connection messages
	// Send property messages
	// Load data files and send data messages
	// Load resource files and send resource messages
	// Send init message to components
	return new Emulation;
}

extern "C" int emulatorSave(Emulation * emulation, char * path)
{
	// Request all properties from components and update DML
	// Save in-memory DML to disk
	return 0;
}

extern "C" void emulatorClose(Emulation * emulation)
{
	// Send close message to components
	// Delete components
	delete emulation;
}

extern "C" int emulatorIoctl(Emulation * emulation,
							 char * componentName, int message, void * data)
{
	// Find component in DML
	// Call component ioctl
	return 0;
}

extern "C" DMLInfo *emulatorGetDMLInfo(Emulation * emulation)
{
//	DMLInfo *dmlInfoParse()
	// Build a DML Info from the XML structure in memory
	return NULL;
}

extern "C" int emulatorAddDML(Emulation * emulation, char * path, DMLConnections * conn)
{
	// Load DML Tree
	// Rename device names, so they don't step on old ones
	// Update reference names in tree and conn
	// Add new tree to old one
	// Go over the connections, and set the inlets to the outlets
	return 0;
}

extern "C" int emulatorRemoveOutlet(Emulation * emulation, char * outletRef)
{
	// Search component with outletRef
	// Get components' inlets
	// Iterate from the beginning with all connected outlets
	return 0;
}
