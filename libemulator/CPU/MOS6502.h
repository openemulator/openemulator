
/**
 * libemulator
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
	
	bool setProperty(string name, string value);
	bool getProperty(string name, string &value);
	bool connect(string name, OEComponent *component);
	
private:
	OEComponent *memory;
	
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
	
	UInt8 p;
	UInt8 a;
	UInt8 x;
	UInt8 y;
	
	UInt8 pendingIRQ;
	UInt8 afterCLI;
	UInt8 irqCount;
	UInt8 overflow;
	
	Int32 icount;
	
	void reset();
	void assertIRQ();
	void assertNMI();
	void execute();
};
