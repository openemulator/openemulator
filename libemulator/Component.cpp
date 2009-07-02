
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

void Component::init()
{
}

void Component::terminate()
{
}

int Component::ioctl(int command, void * data)
{
	return IOCTL_OK;
}

void Component::addObserver(Component * component)
{
	observers.push_back(component);
}

void Component::removeObserver(Component * component)
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

void Component::onNotification(Component * component)
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

int Component::read(int address)
{
	return 0;
}

void Component::write(int address, int value)
{
}
