
/**
 * libemulation
 * Apple I Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Keyboard
 */

#include "OEComponent.h"

class AppleIKeyboard : public OEComponent
{
public:
	AppleIKeyboard();
	
	bool connect(const string &name, OEComponent *component);
	void notify(int notification, OEComponent *component, void *data);
	
	OEUInt8 read(int address);
	
private:
	OEComponent *host;
	OEComponent *pia;
	
	int key;
};
