
/**
 * libemulation
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a DML file.
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include "OEDML.h"

typedef struct
{
	string ref;
	string type;
	string options;
	string label;
} OESetting;

typedef vector<OESetting *> OESettings;

typedef struct
{
	string ref;
	string type;
	string options;
	string label;
	string image;
	
	string connectionLabel;
	
	OESettings settings;
} OEDevice;

typedef vector<OEDevice *> OEDevices;

typedef struct _OEPort
{
	string ref;
	string type;
	string label;
	string image;
	
	struct _OEPort *connection;
	
	OEDevice *device;
} OEPort;

typedef vector<OEPort *> OEPorts;

class OEInfo : public OEDML
{
public:
	OEInfo();
	OEInfo(string path);
	
	bool open(string path);
	bool isOpen();
	void close();
	
	string getLabel();
	string getImage();
	string getDescription();
	string getGroup();
	
	OEDevices *getDevices();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	
private:
	string label;
	string image;
	string description;
	string group;
	
	OEDevices devices;
	OEPorts inlets;
	OEPorts outlets;
	
	void init();
	
	bool analyze();
	
	void analyzeDeviceNode(xmlNodePtr node);
	OEPort *analyzePortNode(xmlNodePtr node, OEDevice *device);
	OESetting *analyzeSettingNode(xmlNodePtr node, string ref);
	
	bool analyzeConnections();
	
	string getConnectionLabel(string ref);
	string getConnectionLabel(OEPort *outlet, vector<string> &visitedRefs);
};

#endif
