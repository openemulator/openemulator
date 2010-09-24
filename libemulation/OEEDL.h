
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

typedef map<string, string> OEConnections;

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
	
	bool addEDL(string path, OEConnections connections);
	bool removeDevice(string id);
	
protected:
	OEPackage *package;
	xmlDocPtr doc;
	
	string getString(int value);
	
	void setNodeProperty(xmlNodePtr node, string name, string value);
	string getNodeProperty(xmlNodePtr node, string name);
	bool hasNodeProperty(xmlNodePtr node, string name);
	
	string getPathExtension(string path);
	bool writeFile(string path, OEData *data);
	bool readFile(string path, OEData *data);
	
	virtual bool update();
	
private:
	bool is_open;
	
	void init();
	
	bool validate();
	bool dump(OEData *data);
};

#endif
