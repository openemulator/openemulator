
/**
 * libemulation
 * Emulation
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#ifndef _OEEMULATION_H
#define _OEEMULATION_H

#include "OEInfo.h"
#include "OEComponent.h"

typedef map<string, OEComponent *> OEComponentsMap;

class OEEmulation : public OEInfo
{
public:
	OEEmulation();
	OEEmulation(string path);
	OEEmulation(string path, string resourcePath);
	~OEEmulation();
	
	bool open(string path);
	void close();
	
	OEComponent *getComponent(string id);
	
protected:
	
private:
	string resourcePath;
	OEComponentsMap componentsMap;
	
	bool setComponent(string id, OEComponent *component);
	
	string parseProperties(string value, string id);
	
	bool build();
	bool buildComponent(string id, string className);
	bool configure();
	bool configureComponent(string id, xmlNodePtr children);
	bool init();
	bool initComponent(string id);
	bool update();
	bool updateComponent(string id, xmlNodePtr children);
	void remove();
	void removeComponent(string id, xmlNodePtr children);
};

#endif
