
/**
 * libemulation
 * Apple Disk II Interface Card
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple Disk II interface card
 */

#include "AppleDiskIIInterfaceCard.h"

AppleDiskIIInterfaceCard::AppleDiskIIInterfaceCard()
{
	controlBus = NULL;
	
	controllerType = false;
}

bool AppleDiskIIInterfaceCard::setValue(string name, string value)
{
	if (name == "controllerType")
		controllerType = (value == "13 Sector Controller");
	else
		return false;
	
	return true;
}

bool AppleDiskIIInterfaceCard::getValue(string name, string &value)
{
	if (name == "controllerType")
		value = controllerType ? "13 Sector Controller" : "16 Sector Controller";
	else
		return false;
	
	return true;
}

bool AppleDiskIIInterfaceCard::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
		controlBus = ref;
	else
		return false;
	
	return true;
}
