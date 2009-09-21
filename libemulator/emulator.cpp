
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

#include "emulator.h"
#include "Emulation.h"
#include "Package.h"

// ioctl's for:
// * next frame
// * update outlets
// * send power, reset, pause messages
// * send config messages
// * do copy, paste, isCopyAvailable
// * get/set video options
// * get/set audio volume
// * play/record audio
// * lock/unlock disk drives
// * mount/unmount disk drives
// * get/set component options
// * send keyboard messages
// * set mouse position and buttons
// * set joystick position and buttons
// * set graphics tablet position and buttons
// * get video frames

// Note: move pause and power off images to libemulator

using namespace std;

DMLPortNode *dmlPortParse(string deviceName,
						  xmlNodePtr node)
{
	DMLPortNode *portNode = new DMLPortNode;
	
	if (!portNode)
		return NULL;
	
	char *ref = (char *) xmlGetProp(node, BAD_CAST "ref");
	if (ref)
	{
		string refstr = ref;
		xmlFree(ref);
		if (refstr.find("::") == string::npos)
			refstr = deviceName + "::" + refstr;
		ref = (char *) xmlCharStrdup(refstr.c_str());
	}
	portNode->ref = (char *) ref;
	portNode->type = (char *) xmlGetProp(node, BAD_CAST "type");
	portNode->subtype = (char *) xmlGetProp(node, BAD_CAST "subtype");
	portNode->label = (char *) xmlGetProp(node, BAD_CAST "label");
	portNode->image = (char *) xmlGetProp(node, BAD_CAST "image");
	
	portNode->next = NULL;
	
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
	
	xmlNodePtr dmlNode = xmlDocGetRootElement(doc);
	dmlInfo->label = (char *) xmlGetProp(dmlNode, BAD_CAST "label");
	dmlInfo->image = (char *) xmlGetProp(dmlNode, BAD_CAST "image");
	dmlInfo->description = (char *) xmlGetProp(dmlNode, BAD_CAST "description");
	dmlInfo->group = (char *) xmlGetProp(dmlNode, BAD_CAST "group");
	dmlInfo->inlets = NULL;
	dmlInfo->outlets = NULL;
	
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
	Package package(path);
	
	if (!package.isOpen())
		return NULL;
	
	DMLInfo *dmlInfo = NULL;
	
	vector<char> data;
	
	if (package.readFile(DMLINFO_FILENAME, data))
	{
		xmlDocPtr doc = xmlReadMemory(&data[0],
									  data.size(),
									  DMLINFO_FILENAME,
									  NULL,
									  0);
		
		if (doc)
			dmlInfo = dmlInfoParse(doc);
		
		xmlFreeDoc(doc);
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

extern "C" int dmlConnectionsAdd(DMLConnections *conn,
								 char *inletRef,
								 char *outletRef)
{
	DMLConnectionNode *node = new DMLConnectionNode;
	node->next = *conn;
	*conn = node;
	
	node->inletRef = (char *) xmlCharStrdup(inletRef);
	node->outletRef = (char *) xmlCharStrdup(outletRef);
	
	return 0;
}

extern "C" void dmlConnectionsFree(DMLConnections *conn)
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

extern "C" DMLEmulation *emulatorOpen(char *emulationPath, char *resourcePath)
{
	Emulation *emulation = new Emulation(emulationPath, resourcePath);
	if (!emulation)
		return NULL;
	
	if (!emulation->isOpen())
	{
		delete emulation;
		emulation = NULL;
	}
	
	return (DMLEmulation *) emulation;
}

extern "C" int emulatorSave(DMLEmulation *dmlEmulation,
							char *path)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	
	return emulation->save(string(path));
}

extern "C" void emulatorClose(DMLEmulation *dmlEmulation)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	
	delete emulation;
}

extern "C" int emulatorIoctl(DMLEmulation *dmlEmulation,
							 char *componentName,
							 int message,
							 void *data)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	
	return emulation->ioctl(string(componentName), message, data);
}

extern "C" DMLInfo *emulatorGetDMLInfo(DMLEmulation *dmlEmulation)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	
	xmlDocPtr dml = emulation->getDML();
	
	return dmlInfoParse(dml);
}

extern "C" int emulatorAddDML(DMLEmulation *dmlEmulation,
							  char *path,
							  DMLConnections *connections)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	map<string, string> connectionsMap;
	
	// Convert map to connectionsMap
	
	return emulation->addDML(string(path), connectionsMap);
}

extern "C" int emulatorRemoveOutlet(DMLEmulation *dmlEmulation,
									char *outletRef)
{
	Emulation *emulation = (Emulation *) dmlEmulation;
	
	return emulation->removeOutlet(string(outletRef));
}
