
/**
 * libemulation
 * MC6821
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 PIA (Peripheral Interface Adapter)
 */

// Notes:
// * The PIA forwards port data read and write requests as calls to
//   read() and write(). Address 0 is used to for port A, address 1
//   is used for port B.
// * A port device can change CA1, CA2, CB1 and CB2 with setCxy.
// * A port device listens to CA2 and CB2 changes by listening to Cx2_DID_CHANGE.
// * The PIA can listen to resets on the main control bus.
// * It can generate an IRQ on the control bus of port A or port B.

#include "OEComponent.h"

// Messages
typedef enum
{
	MC6821_SET_CA1,
	MC6821_SET_CA2,
	MC6821_GET_CA2,
    MC6821_GET_PA,
    
	MC6821_SET_CB1,
	MC6821_SET_CB2,
	MC6821_GET_CB2,
    MC6821_GET_PB,
} MC6821Message;

// Notifications
typedef enum
{
	MC6821_CA2_DID_CHANGE,
	MC6821_CB2_DID_CHANGE,
} MC6821Notification;

#define MC6821_PORTA                0x00
#define MC6821_PORTB                0x01

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
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	
	bool postMessage(OEComponent *component, int message, void *data);
	
	void notify(OEComponent *component, int notification, void *data);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEUInt8 controlA;
	OEUInt8 directionA;
	OEUInt8 dataA;
	bool ca1;
	bool ca2;
    
	OEUInt8 controlB;
	OEUInt8 directionB;
	OEUInt8 dataB;
	bool cb1;
	bool cb2;
	
	OEComponent *controlBus;
	OEComponent *portA;
	OEComponent *controlBusA;
	OEComponent *portB;
	OEComponent *controlBusB;
    
    void setCA2(bool value);
    void setCB2(bool value);
    
	void setControlA(OEUInt8 value);
	void setControlB(OEUInt8 value);
};
