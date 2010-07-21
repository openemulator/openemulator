
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
	
	xmlDocPtr getXMLDoc();
	OEPackage *getPackage();
	
protected:
	OEPackage *package;
	xmlDocPtr doc;
	
private:
	bool openState;
	
	void init();
	
	bool validate();
	
	string getString(int value);
	string getPathExtension(string path);
	bool readFile(string path, OEData *data);
	bool writeFile(string path, OEData *data);
	string getXMLProperty(xmlNodePtr node, string name);
	void setXMLProperty(xmlNodePtr node, string name, string value);
};

#endif
