
/**
 * libemulation
 * MC6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 Peripheral Interface Adapter
 */

#include "MC6821.h"

#include "ControlBus.h"
#include "AddressDecoder.h"

MC6821::MC6821()
{
	controlBus = NULL;
	portA = NULL;
	controlBusA = NULL;
	portB = NULL;
	controlBusB = NULL;
}

bool MC6821::setValue(string name, string value)
{
	if (name == "controlA")
		setControlA(getInt(value));
	else if (name == "directionA")
		directionA = getInt(value);
	else if (name == "dataA")
		dataA = getInt(value);
	else if (name == "ca1")
		ca1 = getInt(value);
	else if (name == "ca2")
		ca2 = getInt(value);
	else if (name == "controlB")
		setControlB(getInt(value));
	else if (name == "directionB")
		directionB = getInt(value);
	else if (name == "dataB")
		dataB = getInt(value);
	else if (name == "cb1")
		cb1 = getInt(value);
	else if (name == "cb2")
		cb2 = getInt(value);
	else
		return false;
	
	return true;
}

bool MC6821::getValue(string name, string &value)
{
	if (name == "controlA")
		value = getHexString(controlA);
	else if (name == "directionA")
		value = getHexString(directionA);
	else if (name == "dataA")
		value = getHexString(dataA);
	else if (name == "ca1")
		value = getHexString(ca1);
	else if (name == "ca2")
		value = getHexString(ca2);
	else if (name == "controlB")
		value = getHexString(controlB);
	else if (name == "directionB")
		value = getHexString(directionB);
	else if (name == "dataB")
		value = getHexString(dataB);
	else if (name == "cb1")
		value = getHexString(cb1);
	else if (name == "cb2")
		value = getHexString(cb2);
	else
		return false;
	
	return true;
}

bool MC6821::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
	{
		setObserver(controlBus, ref, CONTROLBUS_RESET_DID_ASSERT);
		controlBus = ref;
	}
	else if (name == "portA")
		portA = ref;
	else if (name == "controlBusA")
		controlBusA = ref;
	else if (name == "portB")
		portB = ref;
	else if (name == "controlBusB")
		controlBusB = ref;
	else
		return false;
	
	return true;
}

bool MC6821::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case MC6821_SET_CA1:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlA & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && ca1 && !value) ||
				(isLowToHigh && !ca1 && value))
			{
				if (controlA & MC6821_CR_C1ENABLEIRQ)
					setControlA(controlA | MC6821_CR_IRQ1FLAG);
				if ((controlA & 
					(MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE))
					== MC6821_CR_C2OUTPUT)
				{
					int value = 1;
					postMessage(this, MC6821_SET_CA2, &value);
				}
			}
			ca1 = value;
			
			return true;
		}
		case MC6821_SET_CA2:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlA & MC6821_CR_C2LOWTOHIGH);
			if ((!isLowToHigh && ca2 && !value) ||
				(isLowToHigh && !ca2 && value))
			{
				if ((controlA & (MC6821_CR_C2OUTPUT && MC6821_CR_C2ENABLEIRQ))
					== MC6821_CR_C2ENABLEIRQ)
					setControlA(controlA | MC6821_CR_IRQ2FLAG);
			}
			if ((controlA & MC6821_CR_C2OUTPUT) && (ca2 != value))
				OEComponent::notify(this, MC6821_CA2_CHANGED, NULL);
			ca2 = value;
			
			return true;
		}
		case MC6821_GET_CA2:
		{
			int *value = (int *) data;
			*value = ca2;
			
			return true;
		}
		case MC6821_SET_CB1:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlB & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && cb1 && !value) ||
				(isLowToHigh && !cb1 && value))
			{
				if (controlB & MC6821_CR_C1ENABLEIRQ)
					setControlB(controlB | MC6821_CR_IRQ1FLAG);
				if ((controlB & 
					 (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE))
					 == MC6821_CR_C2OUTPUT)
				{
					int value = 0;
					postMessage(this, MC6821_SET_CB2, &value);
				}
			}
			cb1 = value;
			
			return true;
		}
		case MC6821_SET_CB2:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlB & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && cb1 && !value) ||
				(isLowToHigh && !cb1 && value))
			{
				if ((controlB & (MC6821_CR_C2OUTPUT && MC6821_CR_C2ENABLEIRQ))
					== MC6821_CR_C1ENABLEIRQ)
					setControlB(controlB | MC6821_CR_IRQ2FLAG);
			}
			if (cb2 != value)
				OEComponent::notify(this, MC6821_CB2_CHANGED, &value);
			cb2 = value;
			
			return true;
		}
		case MC6821_GET_CB2:
		{
			int *value = (int *) data;
			*value = cb2;
			
			return true;
		}
	}
	
	return OEComponent::postMessage(sender, message, data);
}

void MC6821::notify(OEComponent *component, int notification, void *data)
{
	// Reset did assert
	setControlA(0);
	directionA = 0;
	dataA = 0;
	ca1 = 0;
	ca2 = 0;
	
	setControlB(0);
	directionB = 0;
	dataB = 0;
	cb1 = 0;
	cb2 = 0;
}

OEUInt8 MC6821::read(OEAddress address)
{
	switch(address & 0x3)
	{
		case MC6821_RS_DATAREGISTERA:
			if (controlA & MC6821_CR_DATAREGISTER)
			{
				if (controlA & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
					== MC6821_CR_C2OUTPUT)
				{
					int ca2 = 0;
					postMessage(this, MC6821_SET_CA2, &ca2);
					if (controlA & MC6821_CR_C2ERESTORE)
					{
						ca2 = 1;
						postMessage(this, MC6821_SET_CA2, &ca2);
					}
				}
				setControlA(controlA &
							~(MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG));
				dataA &= directionA;
				dataA |= portA->read(0) & ~directionA;
				return dataA;
			}
			else
				return directionA;
		case MC6821_RS_CONTROLREGISTERA:
			return controlA;
		case MC6821_RS_DATAREGISTERB:
			if (controlB & MC6821_CR_DATAREGISTER)
			{
				setControlB(controlB &
							~(MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG));
				dataB &= directionB;
				dataB |= portB->read(1) & ~directionB;
				return dataB;
			}
			else
				return directionB;
		case MC6821_RS_CONTROLREGISTERB:
			return controlB;
	}
	
	return 0;
}

void MC6821::write(OEAddress address, OEUInt8 value)
{
	switch(address & 0x3)
	{
		case MC6821_RS_DATAREGISTERA:
			if (controlA & MC6821_CR_DATAREGISTER)
			{
				dataA = value;
				portA->write(0, value);
			}
			else
				directionA = value;
			break;
		case MC6821_RS_CONTROLREGISTERA:
			setControlA(value);
			if (value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT) ==
				(MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
			{
				int ca2 = value & MC6821_CR_C2SET;
				postMessage(this, MC6821_SET_CA2, &ca2);
			}
			break;
		case MC6821_RS_DATAREGISTERB:
			if (controlB & MC6821_CR_DATAREGISTER)
			{
				if (controlB & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
					== MC6821_CR_C2OUTPUT)
				{
					int cb2 = 0;
					postMessage(this, MC6821_SET_CB2, &cb2);
					if (controlB & MC6821_CR_C2ERESTORE)
					{
						cb2 = 1;
						postMessage(this, MC6821_SET_CB2, &cb2);
					}
				}
				dataB = value;
				portB->write(1, value);
			}
			else
				directionB = value;
			break;
		case MC6821_RS_CONTROLREGISTERB:
			setControlB(value);
			if (value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
				== (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
			{
				int cb2 = value & MC6821_CR_C2SET;
				postMessage(this, MC6821_SET_CB2, &cb2);
			}
			break;
	}
}

void MC6821::setControlA(int value)
{
	bool wasIRQ = controlA & MC6821_CR_IRQFLAGS;
	controlA = value;
	bool isIRQ = controlA & MC6821_CR_IRQFLAGS;
	
	if (controlBusA)
	{
		if (wasIRQ && !isIRQ)
			controlBusA->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
		else if (!wasIRQ && isIRQ)
			controlBusA->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);
	}
}

void MC6821::setControlB(int value)
{
	bool wasIRQ = controlB & MC6821_CR_IRQFLAGS;
	controlB = value;
	bool isIRQ = controlB & MC6821_CR_IRQFLAGS;
	
	if (controlBusB)
	{
		if (wasIRQ && !isIRQ)
			controlBusB->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
		else if (!wasIRQ && isIRQ)
			controlBusB->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);
	}
}
