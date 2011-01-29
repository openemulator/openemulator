
/**
 * libemulation
 * MOS KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS KIM-1 input/output
 */

#include "MOSKIM1IO.h"
#include "Emulation.h"
#include "CanvasInterface.h"
#include "RS232Interface.h"

MOSKIM1IO::MOSKIM1IO()
{
	emulation = NULL;
	serialPort = NULL;
	audioOut = NULL;
	audioIn = NULL;
	
	canvas = NULL;
}

bool MOSKIM1IO::setValue(string name, string value)
{
	if (name == "viewPath")
		viewPath = value;
	else
		return false;
	
	return true;
}

bool MOSKIM1IO::setRef(string name, OEComponent *ref)
{
	if (name == "emulation")
	{
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_DESTROY_CANVAS,
								   &canvas);
		emulation = ref;
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_CREATE_CANVAS,
								   &canvas);
	}
	else if (name == "serialPort")
	{
		setObserver(serialPort, ref, RS232_DID_RECEIVE_DATA);
		serialPort = ref;
	}
	else if (name == "audioOut")
		audioOut = ref;
	else if (name == "audioIn")
		audioIn = ref;
	else
		return false;
	
	return true;
}

bool MOSKIM1IO::init()
{
	if (!emulation)
	{
		log("property 'emulation' undefined");
		return false;
	}
	
	if (!canvas)
	{
		log("canvas could not be created");
		return false;
	}
	
	if (canvas)
	{
		OEImage frame;
		frame.readFile(viewPath);
		canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
		
		CanvasConfiguration configuration;
		configuration.viewMode = CANVAS_VIEWMODE_FIT_CANVAS;
		configuration.captureMode = CANVAS_CAPTUREMODE_NO_CAPTURE;
		configuration.defaultViewSize = frame.getSize();
		configuration.canvasSize = frame.getSize();
		configuration.contentRect = OEMakeRect(0, 0, 1, 1);
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
	}
	
	return true;
}

void MOSKIM1IO::notify(OEComponent *sender, int notification, void *data)
{
}
