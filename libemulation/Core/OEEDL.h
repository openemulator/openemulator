
/**
 * libemulation
 * OEEDL
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

#ifndef _OEEDL_H
#define _OEEDL_H

#include <libxml/tree.h>

#include "OECommon.h"
#include "OEPackage.h"

#define OE_FILE_PATH_EXTENSION "xml"
#define OE_PACKAGE_PATH_EXTENSION "emulation"
#define OE_PACKAGE_EDL_PATH "info.xml"

typedef struct
{
	string label;
	string image;
	string description;
} OEHeaderInfo;

typedef struct
{
	string id;
	string type;
	string label;
	string image;
} OEPortInfo;
typedef vector<OEPortInfo> OEPortsInfo;

typedef OEPortInfo OEConnectorInfo;
typedef OEPortsInfo OEConnectorsInfo;

typedef vector<string> OEIds;
typedef map<string, string> OEIdMap;

class OEEDL
{
public:
	OEEDL();
	~OEEDL();
	
	bool open(string path);
	bool isOpen();
	bool save(string path);
	void close();
	
	OEHeaderInfo getHeaderInfo();
	OEPortsInfo getFreePortsInfo();
	OEConnectorsInfo getFreeConnectorsInfo();
	
    bool addEDL(string path, OEIdMap connections);
    
    OEIds getDeviceIds();
    
	void setDeviceId(string& id, string deviceId);
	string getDeviceId(string id);
	
protected:
	bool is_open;
	OEPackage *package;
	xmlDocPtr doc;
	
    virtual bool updateEmulation();
    
	string getLocationLabel(string deviceId, vector<string>& visitedIds);
	string getLocationLabel(string deviceId);
    
	string getNodeProperty(xmlNodePtr node, string name);
	bool hasNodeProperty(xmlNodePtr node, string name);
	void setNodeProperty(xmlNodePtr node, string name, string value);
	
private:
	bool validateEmulation();
	bool dumpEmulation(OEData *data);
    
    OEIdMap makeIdMap(OEIds& deviceIds);
    void remapNode(OEIdMap& deviceIdMap, xmlNodePtr node, string property);
    void remapDocument(OEIdMap& deviceIdMap);
    void remapConnections(OEIdMap& connections, OEIdMap& deviceIdMap);
    xmlNodePtr getLastNode(string theDeviceId);
    string followChain(string deviceId, vector<string>& visitedIds);
    xmlNodePtr getInsertionNode(string connectorId);
    bool insertInto(xmlNodePtr dest);
    void connect(OEEDL& edl, OEIdMap& connections);
};

#endif
