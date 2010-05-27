
/**
 * libemulator
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
	bool connect(string name, OEComponent *component);
	void notify(int notification, OEComponent *component, void *data);
	
	int read(int address);
	
private:
	OEComponent *hostKeyboard;
	OEComponent *pia;
	
	int key;
};
