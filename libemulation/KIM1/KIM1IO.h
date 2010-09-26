
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "OEComponent.h"

#include "Host.h"

class KIM1IO : public OEComponent
{
public:
	KIM1IO();
	~KIM1IO();
	
	bool setComponent(string name, OEComponent *component);
	bool setData(string name, OEData *data);
	
	void notify(OEComponent *component, int notification, void *data);
	
private:
	OEComponent *host;
	OEComponent *serialPort;
	OEComponent *audioOut;
	OEComponent *audioIn;
	
	OEData *view;
	HostVideoScreen *screen;
};
