
/**
 * libemulator
 * Z80
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a Z80 microprocessor.
 */

#include "OEComponent.h"


class Z80 : public OEComponent
{
public:
	Z80();
	
	int ioctl(int message, void *data);		
	
private:
	OEComponent *memory;
	OEComponent *port;
	
	// 8080
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair ea;
	
	OEPair af;
	OEPair bc;
	OEPair de;
	OEPair hl;

	// Z80
	OEPair af2;
	OEPair bc2;
	OEPair de2;
	OEPair hl2;
	OEPair ix;
	OEPair iy;
	OEPair wz;
	UINT8 i;
	UINT8 r;
	
	UINT8 rr;
	
	UINT8 iff1;
	UINT8 iff2;
	UINT8 halt;
	UINT8 im;
	
	UINT8 nmi_state;		/* nmi line state */
	UINT8 nmi_pending;		/* nmi pending */
	UINT8 irq_state;		/* irq line state */
	UINT8 after_ei;			/* are we in the EI shadow? */
	
	void execute();
};
