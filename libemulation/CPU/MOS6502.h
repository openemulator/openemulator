
/**
 * libemulation
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor.
 */

#include "OEComponent.h"
#include "ControlBus.h"

class MOS6502 : public OEComponent
{
public:
	MOS6502();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	bool connect(const string &name, OEComponent *component);
	
	void notify(OEComponent *component, int notification, void *data);
	
private:
	OEComponent *memory;
	OEComponent *bus;
	
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
	
	OEUInt8 p;
	OEUInt8 a;
	OEUInt8 x;
	OEUInt8 y;
	
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
