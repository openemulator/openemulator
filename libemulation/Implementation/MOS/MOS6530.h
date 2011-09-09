
/**
 * libemulation
 * MOS6530
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6530 (ROM, RAM, I/O, Timer)
 */

#include "OEComponent.h"

class MOS6530 : public OEComponent
{
public:
	MOS6530();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *controlBus;
	OEComponent *portA;
	OEComponent *portB;
	OEComponent *controlBusB;
	
	OEUInt8 directionA;
	OEUInt8 dataA;
	OEUInt8 directionB;
	OEUInt8 dataB;
};
