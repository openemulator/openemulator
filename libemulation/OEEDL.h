
/**
 * libemulation
 * OEEDL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an EDL document
 */

#ifndef _OEEDL_H
#define _OEEDL_H

#include <libxml/tree.h>

#include "OETypes.h"

#include "OEPackage.h"

#define OE_STANDALONE_EXTENSION "xml"
#define OE_PACKAGE_EXTENSION "emulation"
#define OE_PACKAGE_EDL_FILENAME "info.xml"

typedef map<string, string> OEIdMap;
typedef vector<string> OEIdList;

class OEEDL
{
public:
	OEEDL();
	OEEDL(string path);
	~OEEDL();
	
	bool open(string path);
	bool isOpen();
	bool save(string path);
	void close();
	
	bool addEDL(string path, OEIdMap connectionMap);
	bool removeDevice(string ref);
	
protected:
	OEPackage *package;
	xmlDocPtr doc;
	
	virtual bool update();
	
	string getString(int value);
	void setNodeProperty(xmlNodePtr node, string name, string value);
	bool hasNodeProperty(xmlNodePtr node, string name);
	string getNodeProperty(xmlNodePtr node, string name);
	string getPathExtension(string path);
	bool writeFile(string path, OEData *data);
	bool readFile(string path, OEData *data);
	
	// Helpers
	void log(string message);
	
private:
	bool is_open;
	
	void init();
	
	bool validate();
	bool dump(OEData *data);
	
	OEIdMap buildNameMap(OEIdList deviceIds, OEIdList newDeviceIds);
	void rename(OEIdMap nameMap);
	void rename(OEIdMap nameMap, xmlNodePtr node, string property);
	bool renameConnectionMap(OEIdMap &connectionMap, OEIdMap nameMap);
	void insert(OEEDL *edl, string deviceId);
	void connect();
	
	bool removeConnectedDevices(string deviceId);
	void disconnect(string deviceId);
	void removeElements(string deviceId);
	
	bool hasDevice(string deviceId);
	OEIdList getDeviceIds();
	void setDeviceId(string &ref, string deviceId);
	string getDeviceId(string ref);
};

#endif
