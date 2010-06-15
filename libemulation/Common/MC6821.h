
/**
 * libemulation
 * MC6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 Peripheral Interface Adapter
 */

#include "OEComponent.h"

// Messages
enum
{
	MC6821_SET_CA1,
	MC6821_SET_CA2,
	MC6821_GET_CA2,
	MC6821_SET_CB1,
	MC6821_SET_CB2,
	MC6821_GET_CB2,
};

// Notifications
enum {
	MC6821_CA2_CHANGED,
	MC6821_CB2_CHANGED,
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

class MC6821 : public OEComponent
{
public:
	MC6821();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	bool connect(const string &name, OEComponent *component);
	
	void notify(int notification, OEComponent *component, void *data);
	
	int ioctl(int message, void *data);

	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	int resetNotification;
	OEComponent *reset;
	
	OEComponent *interfaceA;
	int irqANotification;
	OEComponent *irqA;

	OEComponent *interfaceB;
	int irqBNotification;
	OEComponent *irqB;
	
	int controlRegisterA;
	int dataDirectionRegisterA;
	int dataRegisterA;
	int controlRegisterB;
	int dataDirectionRegisterB;
	int dataRegisterB;
	
	bool ca1;
	bool ca2;
	bool cb1;
	bool cb2;
	
	void setControlRegisterA(int value);
	void setControlRegisterB(int value);
};
