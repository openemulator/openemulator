
/**
 * libemulation
 * MOS KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS KIM-1 input/output
 */

#include "OEComponent.h"

class MOSKIM1IO : public OEComponent
{
public:
	MOSKIM1IO();
	~MOSKIM1IO();
	
	bool setData(string name, OEData *data);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
	void notify(OEComponent *sender, int notification, void *data);
	
private:
	OEComponent *emulation;
	OEComponent *serialPort;
	OEComponent *audioOut;
	OEComponent *audioIn;
	
	OEData *view;
	
	OEComponent *canvas;
	string windowFrame;
	string defaultWindowSize;
};
