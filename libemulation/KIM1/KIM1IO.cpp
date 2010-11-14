
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

bool KIM1IO::setRef(string name, OEComponent *id)
{
	if (name == "hostCanvasController")
		hostCanvasController = id;
	else if (name == "serialPort")
	{
		replaceObserver(serialPort, id, RS232_DATA_RECEIVED);
		serialPort = id;
	}
	else if (name == "audioOut")
		audioOut = id;
	else if (name == "audioIn")
		audioIn = id;
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
