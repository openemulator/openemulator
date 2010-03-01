
/**
 * libemulator
 * PIA6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 6821 Peripheral Interface Adapter
 */

#include "OEComponent.h"

enum
{
	PIA6821_RESET = OEIOCTL_USER,
	PIA6821_SET_CA,
	PIA6821_GET_CA,
	PIA6821_SET_CB,
	PIA6821_GET_CB,
};

#define PIA6821_RS_DATAREGISTERA	0x00
#define PIA6821_RS_CONTROLREGISTERA	0x01
#define PIA6821_RS_DATAREGISTERB	0x02
#define PIA6821_RS_CONTROLREGISTERB	0x03

#define PIA6821_CR_C1ENABLEIRQ	0x01
#define PIA6821_CR_C1LOWTOHIGH	0x02
#define PIA6821_CR_DATAREGISTER	0x04
#define PIA6821_CR_C2OUTPUT		0x20
#define PIA6821_CR_C2ENABLEIRQ	0x08	// If C2 is output
#define PIA6821_CR_C2LOWTOHIGH	0x10	// If C2 is output
#define PIA6821_CR_C2STROBEC1	0x08	// If C2 is input and bit 4 is 0
#define PIA6821_CR_SETCX2		0x08	// If C2 is input and bit 4 is 1
#define PIA6821_CR_IRQ2FLAG		0x40
#define PIA6821_CR_IRQ1FLAG		0x80

class PIA6821 : public OEComponent
{
public:
	PIA6821();
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	int controlRegisterA;
	int dataDirectionRegisterA;
	int dataRegisterA;
	int controlRegisterB;
	int dataDirectionRegisterB;
	int dataRegisterB;
	
	OEComponent *interfaceA;
	OEComponent *irqA;
	OEComponent *interfaceB;
	OEComponent *irqB;
	
	void reset();
};
