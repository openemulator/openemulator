
/**
 * libemulator
 * OEInfo
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include <string>
#include <vector>

#include <libxml/tree.h>

#include "OEDefines.h"
#include "OERef.h"

using namespace std;

class OESetting
{
public:
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

typedef vector<OESetting> OESettings;

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
		
		connectedLabel = port.connectedLabel;
		connectedPort = port.connectedPort;
		
		settings = port.settings;
	}
	
	OERef ref;
	string type;
	string category;
	string label;
	string image;
	
	string deviceLabel;
	
	string connectedLabel;
	OEPort *connectedPort;
	
	OESettings settings;
};

typedef vector<OEPort> OEPorts;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	OEInfo(const OEInfo &info);
	
	bool isLoaded();
	
	string getLabel();
	string getImage();
	string getDescription();
	string getGroup();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	
private:
	bool loaded;
	
	string label;
	string image;
	string description;
	string group;
	
	OEPorts inlets;
	OEPorts outlets;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	
	string getNodeProperty(xmlNodePtr node, string key);
	
	OERef getOutletRefForInletRef(xmlDocPtr dml, OERef inletRef);
	OEPort *getOutlet(OERef outletRef);
	string makeLabel(OEPort *outlet, vector<OERef> *refs);
	
	OEPort parsePort(OERef deviceRef,
					 string deviceLabel,
					 string deviceImage,
					 xmlNodePtr node);
	void parse(xmlDocPtr dml);
};

#endif
