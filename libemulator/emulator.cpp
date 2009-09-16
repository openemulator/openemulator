
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

class EmulationClass
{
private:
	vector<Component *> components;
};

extern "C" DMLInfo *dmlInfoRead(char * path)
{
	// Read DML File
	// Load <dml ... data
	// Iterate through <device's
	// Load <inlet's
	// Load <outlet's
	return NULL;
}

extern "C" DMLInfo *dmlInfoReadFromTemplate(char * path)
{
	// Get DML File in memory
	// Load <dml ... data
	// Iterate through <device's
	// Load <inlet's
	// Load <outlet's
	return NULL;
}

void dmlPortFree(DMLPortNode *node)
{
	while (node)
	{
		DMLPortNode *next = node->next;
		
		free(node->ref);
		free(node->type);
		free(node->subtype);
		free(node->label);
		free(node->image);
		delete node;
		
		node = next;
	}
}

extern "C" void dmlInfoFree(DMLInfo * dmlInfo)
{
	dmlPortFree(dmlInfo->inlets);
	dmlPortFree(dmlInfo->outlets);
	
	delete dmlInfo;
}

extern "C" DMLConnections *dmlConnectionNew()
{
	return new DMLConnections;
}

extern "C" int dmlConnectionsAdd(DMLConnections * conn, char * inletRef, char * outletRef)
{
	DMLConnectionNode *node = new DMLConnectionNode;
	node->next = *conn;
	*conn = node;
	
	node->inletRef = strdup(inletRef);
	node->outletRef = strdup(outletRef);
	
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

extern "C" int emulatorIoctl(Emulation * emulation, char * componentName, int message, void * data)
{
	// Find component in DML
	// Get instance
	// Call ioctl
	return 0;
}

extern "C" DMLInfo *emulatorGetDMLInfo(Emulation * emulation)
{
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

/*
Emulation::Emulation()
{
	xmlInitParser();
	
    LIBXML_TEST_VERSION
}

Emulation::~Emulation()
{
	xmlCleanupParser();
	xmlMemoryDump();
}

void printElementNames(xmlNode *curNode)
{
	for (; curNode; curNode = curNode->next)
	{
		printf("node type: %d, name: %s\n", curNode->type, curNode->name);
		
		printElementNames(curNode->children);
	}
}

bool Emulation::readDML(char *dmlData, int dmlDataSize, DMLInfo &dmlInfo)
{
	xmlDocPtr doc;
	
	xmlReadMemory(dmlData, dmlDataSize, NULL, NULL, 0);
	
	if (doc)
	{
		xmlNode *rootElement = xmlDocGetRootElement(doc);
		
		printElementNames(rootElement);
		
		xmlFreeDoc(doc);
	}
	
	return (doc != NULL);
}

bool Emulation::readDML(string path, DMLInfo &dmlInfo)
{
	bool isError = true;

	FILE *dmlFile;
	if ((dmlFile = fopen(path.c_str(), "rb")) != NULL)
	{
		fseek(dmlFile, 0, SEEK_END);
		int dmlFileSize = ftell(dmlFile);
		fseek(dmlFile, 0, SEEK_SET);
		
		char dmlData[dmlFileSize];
		if (fread(dmlData, 1, dmlFileSize, dmlFile) == dmlFileSize)
			isError = readDML(dmlData, dmlFileSize, dmlInfo);
		
		fclose(dmlFile);
	}
	
	return !isError;
}

bool Emulation::readTemplate(string path, DMLInfo &dmlInfo)
{
	bool isError = true;
	
	struct zip *zipFile;
	if ((zipFile = zip_open(path.c_str(), 0, NULL)) != NULL)
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
					isError = readDML(dmlData, dmlFileSize, dmlInfo);

				zip_fclose(dmlFile);
			}
		}
		
		zip_close(zipFile);
	}
	
	return !isError;
}

bool Emulation::open(string emulationPath)
{
	return true;
}

bool Emulation::runFrame()
{
	return true;
}

bool Emulation::save(string emulationPath)
{
	return true;
}

bool Emulation::ioctl(string componentName, int message, void * data)
{
	return 0;
}

bool Emulation::getOutlets(vector<DMLOutlet> &outlets)
{
	return true;
}

bool Emulation::getInlets(vector<DMLInlet> &inlets)
{
	return true;
}

bool Emulation::getAvailableDMLs(map<string, DMLInfo> &dmls,
								 vector<string> &availableDMLs)
{
	return true;
}

bool Emulation::getAvailableInlets(DMLOutlet &outlet,
								   vector<DMLInlet> &availableInlets)
{
	return true;
}

bool Emulation::addDML(string dmlPath,
					   map<string, string> outletInletMap)
{
	return true;
}

void Emulation::removeDevicesOnOutlet(string outletName)
{
}
*/
