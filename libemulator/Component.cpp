
/**
 * libemulator
 * Component interface
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "Component.h"

Component::Component()
{
}

Component::~Component()
{
}

int Component::ioctl(int command, void * data)
{
	return 0;
}

int Component::read(int address)
{
	return 0;
}

void Component::write(int address, int value)
{
}

void Component::addObserver(Component *component)
{
	observers.push_back(component);
}

void Component::removeObserver(Component *component)
{
	vector<Component *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
	{
		if (*iterator == component)
			observers.erase(iterator);
	}
}

void Component::onNotification(Component *component)
{
}

void Component::postNotification()
{
	vector<Component *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
		(*iterator)->onNotification(this);
}
