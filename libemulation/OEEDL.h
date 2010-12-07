
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

#define OE_FILE_PATH_EXTENSION "xml"
#define OE_PACKAGE_PATH_EXTENSION "emulation"
#define OE_PACKAGE_EDL_PATH "info.xml"

typedef map<string, string> OEIdMap;
typedef vector<string> OEIdList;

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

class OEEDL
{
public:
	OEEDL();
	~OEEDL();
	
	bool open(string path);
	bool isOpen();
	void close();
	
	OEHeaderInfo getHeaderInfo();
	void setWindowFrame(string windowFrame);
	string getWindowFrame();
	OEPortsInfo getFreePortsInfo();
	OEConnectorsInfo getFreeConnectorsInfo();
	
protected:
	bool is_open;
	OEPackage *package;
	xmlDocPtr doc;
	
	void setNodeProperty(xmlNodePtr node, string name, string value);
	bool hasNodeProperty(xmlNodePtr node, string name);
	string getNodeProperty(xmlNodePtr node, string name);
	
	bool writeFile(string path, OEData *data);
	bool readFile(string path, OEData *data);
	
	string getPathExtension(string path);
	
private:
	bool validateEmulation();
	
	void edlLog(string message);
};

#endif
