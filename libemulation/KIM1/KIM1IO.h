
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "OEComponent.h"

#include "HostInterface.h"

class KIM1IO : public OEComponent
{
public:
	KIM1IO();
	~KIM1IO();
	
	bool setRef(string name, OEComponent *ref);
	bool setData(string name, OEData *data);
	
	void notify(OEComponent *sender, int notification, void *data);
	
private:
	OEComponent *controlBus;
	OEComponent *serialPort;
	OEComponent *audioOut;
	OEComponent *audioIn;
	
	OEData *view;
};
