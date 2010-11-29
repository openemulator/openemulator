
/**
 * libemulation
 * MOS KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS KIM-1 input/output
 */

#include "OEComponent.h"
#include "HostCanvasInterface.h"

class MOSKIM1IO : public OEComponent
{
public:
	MOSKIM1IO();
	~MOSKIM1IO();
	
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	bool setData(string name, OEData *data);
	bool init();
	
	void notify(OEComponent *sender, int notification, void *data);
	
private:
	OEComponent *hostEmulationController;
	OEComponent *serialPort;
	OEComponent *audioOut;
	OEComponent *audioIn;
	
	OEData *view;
	
	OEComponent *canvas;
	string windowFrame;
	string defaultWindowSize;
};
