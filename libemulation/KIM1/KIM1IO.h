
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "OEComponent.h"

class KIM1IO : public OEComponent
{
public:
	KIM1IO();
	~KIM1IO();
	
	bool setResource(const string &name, OEData *data);
	bool connect(const string &name, OEComponent *component);

private:
	OEComponent *host;
	OEComponent *audioOut;
	OEComponent *audioIn;
	OEComponent *terminal;
	
	OEData *view;
};
