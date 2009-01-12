
/**
 * libemulator
 * Component interface
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#define COMPONENT_MAXEVENTS	64

#include <string>
#include <vector>
#include <map>

#include <stdint.h>

using namespace std;

struct dependencyType {
	char * componentName;
	class Component * component;
};

class Component {
public:
	Component();
	virtual ~Component();
	
	string getName();
	void setParameters(map<string, string> parameters);
	map<string, string> getParameters();
	
	virtual void onDependenciesLinked();
	
	virtual void sendMessage(void * messageData);
	
	virtual void onEvent(string &source, uint32_t eventIndex);
	void attachListener(uint32_t eventIndex, class Component &component);
	void detachListener(uint32_t eventIndex, class Component &component);
	
	virtual uint8_t readMemory(uint32_t address);
	virtual void writeMemory(uint32_t address, uint8_t value);
	
protected:
	void setEvent(uint32_t eventIndex);
	
private:
	string componentName;
	vector<Component> eventListenerList[COMPONENT_MAXEVENTS];
};

#endif
