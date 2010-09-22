
/**
 * libemulation
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a EDL file.
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include "OEEDL.h"

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
	string name;
	string type;
	string options;
	string label;
	string image;
	
	string connectionLabel;
	
	OESettings settings;
} OEDevice;

typedef vector<OEDevice *> OEDevices;

typedef struct OEPortStruct
{
	string ref;
	string type;
	string label;
	string image;
	
	struct OEPortStruct *connection;
	
	OEDevice *device;
} OEPort;

typedef vector<OEPort *> OEPorts;

class OEInfo : public OEEDL
{
public:
	OEInfo();
	OEInfo(const string &path);
	~OEInfo();
	
	bool open(const string &path);
	
	bool addEDL(const string &path, OEConnections &connections);
	bool removeDevice(const string &deviceName);
	
	string getType();
	string getLabel();
	string getImage();
	string getDescription();
	
	OEDevices *getDevices();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	
private:
	string type;
	string label;
	string image;
	string description;
	
	OEDevices devices;
	OEPorts inlets;
	OEPorts outlets;
	
	void init();
	
	bool analyze();
	
	void analyzeHeader(xmlNodePtr node);
	void analyzeDevices(xmlNodePtr node);
	
	void addDevice(xmlNodePtr node);
	void addSetting(xmlNodePtr node, OEDevice *device);
	void addPort(xmlNodePtr node, OEDevice *device, OEPorts &ports);
	void removeDevices();
	void removePorts(OEPorts &ports);
	
	bool analyzeConnections();
	string buildConnectionLabel(string ref);
	string buildConnectionLabel(OEPort *outlet, vector<string> &visitedRefs);
	bool findCircularConnection(string ref, vector<string> &visitedRefs);
};

#endif
