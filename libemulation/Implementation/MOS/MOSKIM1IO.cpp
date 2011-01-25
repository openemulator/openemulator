
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
		OEImage *frame = NULL;
		OESize frameSize;
		canvas->postMessage(this, CANVAS_REQUEST_FRAME, &frame);
		frame->readFile(viewPath);
		frameSize = frame->getSize();
		canvas->postMessage(this, CANVAS_RETURN_FRAME, &frame);
		
		CanvasConfiguration configuration;
		configuration.size = frameSize;
		configuration.zoomToFit = false;
		configuration.captureMode = CANVAS_CAPTUREMODE_NO_CAPTURE;
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
	}
	
	return true;
}

void MOSKIM1IO::notify(OEComponent *sender, int notification, void *data)
{
}
