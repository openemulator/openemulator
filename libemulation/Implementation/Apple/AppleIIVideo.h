
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II video
 */

#include "OEComponent.h"

class AppleIIVideo : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *ram;
	OEComponent *charset;
	OEComponent *charsetSocket;
	OEComponent *monitor;
	
	bool palTiming;
	string characterSet;
};
