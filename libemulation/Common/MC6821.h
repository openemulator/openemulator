
/**
 * libemulation
 * MC6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 Peripheral Interface Adapter
 */

#include "OEComponent.h"

// Notifications
enum {
	MC6821_CA2_CHANGED,
	MC6821_CB2_CHANGED,
};

// Events
enum
{
	MC6821_SET_CA1,
	MC6821_SET_CA2,
	MC6821_GET_CA2,
	MC6821_SET_CB1,
	MC6821_SET_CB2,
	MC6821_GET_CB2,
};

#define MC6821_RS_DATAREGISTERA		0x00
#define MC6821_RS_CONTROLREGISTERA	0x01
#define MC6821_RS_DATAREGISTERB		0x02
#define MC6821_RS_CONTROLREGISTERB	0x03

#define MC6821_CR_C1ENABLEIRQ		0x01
#define MC6821_CR_C1LOWTOHIGH		0x02
#define MC6821_CR_DATAREGISTER		0x04
#define MC6821_CR_C2ENABLEIRQ		0x08	// If C2OUTPUT is clear
#define MC6821_CR_C2LOWTOHIGH		0x10	// If C2OUTPUT is clear
#define MC6821_CR_C2ERESTORE		0x08	// If C2OUTPUT is set and C2DIRECT is clear
#define MC6821_CR_C2SET				0x08	// If C2OUTPUT is set and C2DIRECT is set
#define MC6821_CR_C2DIRECT			0x10	// If C2OUTPUT is set
#define MC6821_CR_C2OUTPUT			0x20
#define MC6821_CR_IRQ2FLAG			0x40
#define MC6821_CR_IRQ1FLAG			0x80

#define MC6821_CR_IRQFLAGS			(MC6821_CR_IRQ2FLAG | MC6821_CR_IRQ1FLAG)

class MC6821 : public OEComponent
{
public:
	MC6821();
	
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setComponent(string name, OEComponent *component);
	
	void notify(OEComponent *component, int notification, void *data);
	
	bool postEvent(OEComponent *component, int message, void *data);

	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *controlBus;
	
	OEComponent *portA;
	int controlA;
	int directionA;
	int dataA;
	int ca1;
	int ca2;
	OEComponent *controlBusA;

	OEComponent *portB;
	int controlB;
	int directionB;
	int dataB;
	int cb1;
	int cb2;
	OEComponent *controlBusB;
	
	void setControlA(int value);
	void setControlB(int value);
};
