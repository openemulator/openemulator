
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

#include <iostream>
#include <string>
#include <vector>

#include "OEDML.h"
#include "OERef.h"

class OESetting
{
public:
	OESetting()
	{
	}
	OESetting(const OESetting &setting)
	{
		ref = setting.ref;
		type = setting.type;
		options = setting.options;
		label = setting.label;
	}
	
	OERef ref;
	string type;
	string options;
	string label;
};

class OEPort
{
public:
	OEPort()
	{
	}
	OEPort(const OEPort &port)
	{
		ref = port.ref;
		type = port.type;
		category = port.category;
		label = port.label;
		image = port.image;
		
		deviceLabel = port.deviceLabel;
		
		connectionLabel = port.connectionLabel;
		connectionPort = port.connectionPort;
	}
	
	OERef ref;
	string type;
	string category;
	string label;
	string image;
	
	string deviceLabel;
	
	string connectionLabel;
	OEPort *connectionPort;
};

typedef vector<OESetting> OESettings;
typedef vector<OEPort> OEPorts;

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
	
	OEPorts *getInlets();
	OEPorts *getOutlets();
	OESettings *getSettings();
	
protected:
	void analyze();
	void analyzeConnections();
	void analyzeLabels();
	
	xmlNodePtr getNodeForRef(xmlDocPtr doc, OERef ref);
	OERef getOutletForInlet(xmlDocPtr doc, OERef ref);
	OEPort *getPortForOutlet(OERef ref);
	string getConnectionLabel(OEPort *outletPort, vector<OERef> &visitedRefs);
	
	string getString(int value);
	string getXMLProperty(xmlNodePtr node, string name);
	void setXMLProperty(xmlNodePtr node, string name, string value);
	
private:
	string label;
	string image;
	string description;
	string group;
	
	OEPorts inlets;
	OEPorts outlets;
	OESettings settings;
	
	void analyzeDevice(xmlNodePtr node);
	OEPort analyzePort(xmlNodePtr node, OERef ref, string label, string image);
	OESetting analyzeSetting(xmlNodePtr node, OERef ref);
};

#endif
