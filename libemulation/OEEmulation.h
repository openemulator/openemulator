
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
	OEEmulation(const string &path);
	OEEmulation(const string &path, const string &resourcePath);
	~OEEmulation();
	
	bool open(const string &path);
	void close();
	
	OEComponent *getComponent(const string &id);
	
protected:
	
private:
	string resourcePath;
	OEComponentsMap componentsMap;
	
	bool setComponent(const string &id, OEComponent *component);
	
	string parseProperties(string value, const string &id);
	
	bool build();
	bool buildComponent(const string &id, const string &className);
	bool configure();
	bool configureComponent(const string &id, xmlNodePtr children);
	bool init();
	bool initComponent(const string &id);
	bool update();
	bool updateComponent(const string &id, xmlNodePtr children);
	void remove();
	void removeComponent(const string &id, xmlNodePtr children);
};

#endif
