
/**
 * libemulator
 * Emulation Template
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _EMULATIONTEMPLATE_H
#define _EMULATIONTEMPLATE_H

#include <string>
#include <map>

#include <libxml/tree.h>

#include "Component.h"
#include "Package.h"

using namespace std;

class EmulationTemplate
{
public:
	EmulationTemplate(string path);
	EmulationTemplate(xmlDocPtr dml);
	~EmulationTemplate();
	
	map<string, string> getProperties();
	map<string, string> getInlets();
	map<string, string> getOutlets();
	
private:
	map<string, string> properties;
	map<string, string> inlets;
	map<string, string> outlets;
};

#endif
