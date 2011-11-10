
/**
 * libemulation
 * PIA interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the PIA interface
 */

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
} PIAMessage;

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
