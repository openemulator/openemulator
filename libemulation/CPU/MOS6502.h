
/**
 * libemulation
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor.
 */

#include "OEComponent.h"

class MOS6502 : public OEComponent
{
public:
	MOS6502();
	
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *id);
	
	void notify(OEComponent *sender, int notification, void *data);
	
private:
	OEComponent *memoryBus;
	OEComponent *controlBus;
	
	OEPair ppc;
	
	OEUInt8 a;
	OEUInt8 x;
	OEUInt8 y;
	OEUInt8 p;
	
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
	
	OEUInt8 pendingIRQ;
	OEUInt8 afterCLI;
	OEUInt8 irqCount;
	OEUInt8 overflow;
	
	OEInt32 icount;
	
	void reset();
	void assertIRQ();
	void assertNMI();
	void execute();
};
