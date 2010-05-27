
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
		
	UINT8 a;
	UINT8 x;
	UINT8 y;
	UINT8 p;
		
	int irqCount;
		
	UINT8 pendingIRQ;
	UINT8 afterCLI;
	UINT8 soState;
	
	int icount;
		
	void execute();
};
