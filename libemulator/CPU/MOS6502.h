
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
	
	int ioctl(int message, void *data);		
	
private:
	OEComponent *memory;
	
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
	
	UINT8 p;
	UINT8 a;
	UINT8 x;
	UINT8 y;
	
	UINT8 pendingIRQ;
	UINT8 afterCLI;
	UINT8 irqCount;
	UINT8 overflow;
	
	INT32 icount;
	
	void reset();
	void assertIRQ();
	void assertNMI();
	void execute();
};
