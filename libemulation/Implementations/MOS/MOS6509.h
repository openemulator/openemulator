
/**
 * libemulator
 * MOS6509
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6509 microprocessor.
 */

#include "OEComponent.h"

class MOS6509 : public OEComponent
{
public:
	MOS6509();
	
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
	
	// 4 bits, addressed over address 0
	OEPair pcBank;	
	// 4 bits, addressed over address 1
	OEPair indBank;

	int irqCount;

	UINT8 pendingIRQ;
	UINT8 afterCLI;
	UINT8 soState;
	
	int icount;
	
	void execute();
};
