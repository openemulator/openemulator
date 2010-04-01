
/**
 * libemulator
 * W65C02S
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a W65C02S microprocessor.
 */

#include "OEComponent.h"

class W65C02S : public OEComponent
{
public:
	W65C02S();
		
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
		
	UINT8 pending_irq;
	UINT8 after_cli;
	UINT8 nmi_state;
	UINT8 so_state;
	
	int icount;
		
	void execute();
};
