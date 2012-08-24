
/**
 * libemulation
 * MC6821
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an MC6821 PIA (Peripheral Interface Adapter)
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
    MC6821_GET_PA,
	MC6821_SET_CA1,
	MC6821_SET_CA2,
	MC6821_GET_CA2,
    
    MC6821_GET_PB,
	MC6821_SET_CB1,
	MC6821_SET_CB2,
	MC6821_GET_CB2,
} MC6821Message;

// Notifications
typedef enum
{
	MC6821_CA2_DID_CHANGE,
	MC6821_CB2_DID_CHANGE,
} MC6821Notification;

class MC6821 : public OEComponent
{
public:
	MC6821();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	
	bool postMessage(OEComponent *component, int message, void *data);
	
	void notify(OEComponent *component, int notification, void *data);
	
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEAddress addressA;
	OEChar controlA;
	OEChar ddrA;
	OEChar dataA;
	bool ca1;
	bool ca2;
    
    OEAddress addressB;
	OEChar controlB;
	OEChar ddrB;
	OEChar dataB;
	bool cb1;
	bool cb2;
	
	OEComponent *controlBus;
	OEComponent *portA;
	OEComponent *controlBusA;
	OEComponent *portB;
	OEComponent *controlBusB;
    
    void setCA2(bool value);
    void setCB2(bool value);
    
	void setControlA(OEChar value);
	void setControlB(OEChar value);
};
