
/**
 * libemulator
 * RP2A03
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a Ricoh 2A03 microprocessor.
 */

#include "OEComponent.h"

class RP2A03 : public OEComponent
{
public:
	RP2A03();
		
	int ioctl(int message, void *data);		
		
private:
	OEComponent *memory;
		
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
		
	UINT8 a;
	UINT8 x;
	UINT8 y;
	UINT8 p;
		
	int irqCount;
		
	UINT8 pendingIRQ;
	UINT8 afterCLI;
	UINT8 overflow;
	
	int icount;
		
	void execute();
};
