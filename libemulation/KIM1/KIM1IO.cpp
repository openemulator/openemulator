
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "KIM1IO.h"
#include "Terminal.h"

KIM1IO::KIM1IO()
{
	terminal = NULL;
	audioOut = NULL;
	audioIn	= NULL;
}

KIM1IO::~KIM1IO()
{
	delete view;
}

bool KIM1IO::setResource(const string &name, OEData *data)
{
	if (name == "view")
		view = data;
	else
		return false;
	
	return true;
}

bool KIM1IO::connect(const string &name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else if (name == "terminal")
	{
		if (terminal)
			terminal->removeObserver(this, TERMINAL_RECEIVED_CHAR);
		terminal = component;
		if (terminal)
			terminal->addObserver(this, TERMINAL_RECEIVED_CHAR);
	}
	else if (name == "audioOut")
		audioOut = component;
	else if (name == "audioIn")
		audioIn = component;
	else
		return false;
	
	return true;
}
