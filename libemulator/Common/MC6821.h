
/**
 * libemulator
 * MC6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 Peripheral Interface Adapter
 */

#include "OEComponent.h"

enum
{
	MC6821_RESET = OEIOCTL_USER,
	MC6821_SET_CA1,
	MC6821_SET_CA2,
	MC6821_GET_CA2,
	MC6821_SET_CB1,
	MC6821_SET_CB2,
	MC6821_GET_CB2,
};

#define MC6821_RS_OUTPUTREGISTERA	0x00
#define MC6821_RS_CONTROLREGISTERA	0x01
#define MC6821_RS_OUTPUTREGISTERB	0x02
#define MC6821_RS_CONTROLREGISTERB	0x03

#define MC6821_CR_C1ENABLEIRQ		0x01
#define MC6821_CR_C1LOWTOHIGH		0x02
#define MC6821_CR_OUTPUTREGISTER	0x04
#define MC6821_CR_C2ENABLEIRQ		0x08	// If C2OUTPUT is clear
#define MC6821_CR_C2LOWTOHIGH		0x10	// If C2OUTPUT is clear
#define MC6821_CR_C2ERESTORE		0x08	// If C2OUTPUT is set and C2DIRECT is clear
#define MC6821_CR_C2SET				0x08	// If C2OUTPUT is set and C2DIRECT is set
#define MC6821_CR_C2DIRECT			0x10	// If C2OUTPUT is set
#define MC6821_CR_C2OUTPUT			0x20
#define MC6821_CR_IRQ2FLAG			0x40
#define MC6821_CR_IRQ1FLAG			0x80

#define MC6821_AD_DATA				0x00
#define MC6821_AD_C2				0x01

class MC6821 : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEComponent *interfaceA;
	OEComponent *irqA;
	OEComponent *interfaceB;
	OEComponent *irqB;
	
	int controlRegisterA;
	int dataDirectionRegisterA;
	int outputRegisterA;
	int controlRegisterB;
	int dataDirectionRegisterB;
	int outputRegisterB;
	
	bool ca1;
	bool ca2;
	bool cb1;
	bool cb2;
	
	void reset();
	void setControlRegisterA(int value);
	void setControlRegisterB(int value);
};
