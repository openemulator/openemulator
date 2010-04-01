
/**
 * libemulator
 * CSG65CE02
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a California Semiconductor Group 65CE02 microprocessor.
 */

#include "OEComponent.h"

class CSG65CE02 : public OEComponent
{
public:
	CSG65CE02();
		
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
