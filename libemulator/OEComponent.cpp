
/**
 * libemulator
 * OEComponent
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "OEComponent.h"

OEComponent::OEComponent()
{
}

OEComponent::~OEComponent()
{
}

int OEComponent::ioctl(int command, void *data)
{
	return 0;
}

int OEComponent::read(int address)
{
	return 0;
}

void OEComponent::write(int address, int value)
{
}

void OEComponent::addObserver(OEComponent *component)
{
	observers.push_back(component);
}

void OEComponent::removeObserver(OEComponent *component)
{
	vector<OEComponent *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
	{
		if (*iterator == component)
			observers.erase(iterator);
	}
}

void OEComponent::onNotification(OEComponent *component)
{
}

void OEComponent::postNotification()
{
	vector<OEComponent *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
		(*iterator)->onNotification(this);
}
