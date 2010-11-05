
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "KIM1IO.h"

#include "RS232Interface.h"

KIM1IO::KIM1IO()
{
	controlBus = NULL;
	serialPort = NULL;
	audioOut = NULL;
	audioIn = NULL;
	
	view = NULL;
}

KIM1IO::~KIM1IO()
{
	delete view;
}

bool KIM1IO::setRef(string name, OEComponent *ref)
{
	if (name == "serialPort")
	{
		if (serialPort)
			serialPort->removeObserver(this, RS232_DATA_RECEIVED);
		serialPort = ref;
		if (serialPort)
			serialPort->addObserver(this, RS232_DATA_RECEIVED);
	}
	else if (name == "audioOut")
		audioOut = ref;
	else if (name == "audioIn")
		audioIn = ref;
	else
		return false;
	
	return true;
}

bool KIM1IO::setData(string name, OEData *data)
{
	if (name == "view")
		view = data;
	else
		return false;
	
	return true;
}

void KIM1IO::notify(OEComponent *sender, int notification, void *data)
{

}
