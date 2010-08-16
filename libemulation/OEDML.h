
/**
 * libemulation
 * OEDML
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Encapsulates a DML document.
 */

#ifndef _OEDML_H
#define _OEDML_H

#include <libxml/tree.h>

#include "OETypes.h"

#include "OEPackage.h"

#define OE_STANDALONE_EXTENSION "xml"
#define OE_PACKAGE_EXTENSION "emulation"
#define OE_PACKAGE_DML_FILENAME "info.xml"

#define OE_DEVICE_SEPARATOR ":"
#define OE_CONNECTION_SEPARATOR "."

typedef map<string, string> OEConnections;

class OEDML
{
public:
	OEDML();
	OEDML(string path);
	~OEDML();
	
	bool open(string path);
	bool isOpen();
	bool save(string path);
	void close();
	
	bool addDML(string path, OEConnections &connections);
	bool removeDevice(string deviceName);
	
protected:
	OEPackage *package;
	xmlDocPtr doc;
	
	virtual void update();
	virtual void removeDevice(xmlNodePtr deviceNode);
	
	xmlNodePtr getDeviceNode(string deviceName);
	xmlNodePtr getConnectionNode(xmlNodePtr deviceNode, string ref);
	
	string getNodeName(xmlNodePtr node);
	string getNodeRef(xmlNodePtr node, string deviceName);
	
	string getDeviceName(string ref);
	string getComponentName(string ref);
	string getConnectionName(string ref);
	
	string getString(int value);
	string getPathExtension(string path);
	
	bool readFile(string path, OEData *data);
	bool writeFile(string path, OEData *data);
	
	string getNodeProperty(xmlNodePtr node, string name);
	void setNodeProperty(xmlNodePtr node, string name, string value);
	
private:
	bool is_open;
	
	void init();
	
	bool validate();
	bool dump(OEData *data);
	
	xmlNodePtr getChildNodeWithName(xmlNodePtr node, string elementName, string name);
	
	string filterName(string name);
	string filterRef(string name);
};

#endif
