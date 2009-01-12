
/**
 * libemulator
 * Component interface
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "Component.h"

Component::Component(map<string,string> &parameters)
{
	componentName = parameters["id"];
}

Component::~Component()
{
}

string Component::getName()
{
	return componentName;
}

void Component::onDependenciesLinked()
{
}

void Component::sendMessage(void * message)
{
}

void Component::attachListener(uint32_t eventIndex, Component &component)
{
	eventListenerList[eventIndex].push_back(component);
}

void Component::detachListener(uint32_t eventIndex, Component &component)
{
	vector<Component> &eventListener = eventListenerList[eventIndex];
	vector<Component>::iterator iterator;
	for (iterator = eventListener.begin();
		 iterator != eventListener.end();
		 iterator++)
	{
		if (iterator->getName() == component.getName())
			eventListener.erase(iterator);
	}
}

void Component::onEvent(string &source, uint32_t eventIndex)
{
}

void Component::setEvent(uint32_t eventIndex)
{
	vector<Component> &eventListener = eventListenerList[eventIndex];
	vector<Component>::iterator iterator;
	for (iterator = eventListener.begin();
		 iterator != eventListener.end();
		 iterator++)
		iterator->onEvent(componentName, eventIndex);
}

uint8_t Component::readMemory(uint32_t address)
{
	return 0;
}

void Component::writeMemory(uint32_t address, uint8_t value)
{
}
