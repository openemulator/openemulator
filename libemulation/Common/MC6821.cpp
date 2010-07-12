
/**
 * libemulation
 * MC6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MC6821 Peripheral Interface Adapter
 */

#include "MC6821.h"

MC6821::MC6821()
{
	interfaceA = NULL;
	interfaceB = NULL;
	
	irqANotification = 0;
	irqA = NULL;

	irqBNotification = 0;
	irqB = NULL;
	
	resetNotification = 0;
	reset = NULL;
}

void MC6821::setControlRegisterA(int value)
{
	bool wasIRQ = controlRegisterA & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	controlRegisterA = value;
	bool isIRQ = controlRegisterA & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	
	if (irqA)
	{
		if (wasIRQ && !isIRQ)
		{
			bool value = false;
			irqA->postNotification(irqANotification, &value);
		}
		else if (!wasIRQ && isIRQ)
		{
			bool value = true;
			irqA->postNotification(irqANotification, &value);
		}
	}
}

void MC6821::setControlRegisterB(int value)
{
	bool wasIRQ = controlRegisterB & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	controlRegisterB = value;
	bool isIRQ = controlRegisterB & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	
	if (irqB)
	{
		if (wasIRQ && !isIRQ)
		{
			bool value = false;
			irqB->postNotification(irqBNotification, &value);
		}
		else if (!wasIRQ && isIRQ)
		{
			bool value = true;
			irqB->postNotification(irqBNotification, &value);
		}
	}
}

bool MC6821::setProperty(const string &name, const string &value)
{
	if (name == "controlRegisterA")
		setControlRegisterA(getInt(value));
	else if (name == "dataDirectionRegisterA")
		dataDirectionRegisterA = getInt(value);
	else if (name == "dataRegisterA")
		dataRegisterA = getInt(value);
	else if (name == "controlRegisterB")
		setControlRegisterB(getInt(value));
	else if (name == "dataDirectionRegisterB")
		dataDirectionRegisterB = getInt(value);
	else if (name == "dataRegisterB")
		dataRegisterB = getInt(value);
	else if (name == "ca1")
		ca1 = getInt(value);
	else if (name == "ca2")
		ca2 = getInt(value);
	else if (name == "cb1")
		cb1 = getInt(value);
	else if (name == "cb2")
		cb2 = getInt(value);
	else if (name == "irqANotification")
		irqANotification = getInt(value);
	else if (name == "irqBNotification")
		irqBNotification = getInt(value);
	else if (name == "resetNotification")
		resetNotification = getInt(value);
	else
		return false;
	
	return true;
}

bool MC6821::getProperty(const string &name, string &value)
{
	if (name == "controlRegisterA")
		value = getHex(controlRegisterA);
	else if (name == "dataDirectionRegisterA")
		value = getHex(dataDirectionRegisterA);
	else if (name == "dataRegisterA")
		value = getHex(dataRegisterA);
	else if (name == "controlRegisterB")
		value = getHex(controlRegisterB);
	else if (name == "dataDirectionRegisterB")
		value = getHex(dataDirectionRegisterB);
	else if (name == "dataRegisterB")
		value = getHex(dataRegisterB);
	else if (name == "ca1")
		value = getString(ca1);
	else if (name == "ca2")
		value = getString(ca2);
	else if (name == "cb1")
		value = getString(cb1);
	else if (name == "cb2")
		value = getString(cb2);
	else
		return false;
	
	return true;
}

bool MC6821::connect(const string &name, OEComponent *component)
{
	if (name == "interfaceA")
		interfaceA = component;
	else if (name == "interfaceB")
		interfaceB = component;
	else if (name == "irqA")
		irqA = component;
	else if (name == "irqB")
		irqB = component;
	else if (name == "reset")
	{
		if (reset)
			reset->removeObserver(this, resetNotification);
		reset = component;
		if (reset)
			reset->addObserver(this, resetNotification);
	}
	else
		return false;
	
	return true;
}

void MC6821::notify(int notification, OEComponent *component, void *data)
{
	setControlRegisterA(0);
	dataDirectionRegisterA = 0;
	dataRegisterA = 0;
	setControlRegisterB(0);
	dataDirectionRegisterB = 0;
	dataRegisterB = 0;
	
	ca1 = false;
	ca2 = false;
	cb1 = false;
	cb2 = false;
}

int MC6821::ioctl(int message, void *data)
{
	switch (message)
	{
		case MC6821_SET_CA1:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlRegisterA & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && ca1 && !value) ||
				(isLowToHigh && !ca1 && value))
			{
				if (controlRegisterA & MC6821_CR_C1ENABLEIRQ)
					setControlRegisterA(controlRegisterA | MC6821_CR_IRQ1FLAG);
				if ((controlRegisterA & 
					(MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE))
					== MC6821_CR_C2OUTPUT)
				{
					int value = 1;
					ioctl(MC6821_SET_CA2, &value);
				}
			}
			ca1 = value;
			
			return true;
		}
		case MC6821_SET_CA2:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlRegisterA & MC6821_CR_C2LOWTOHIGH);
			if ((!isLowToHigh && ca2 && !value) ||
				(isLowToHigh && !ca2 && value))
			{
				if ((controlRegisterA & (MC6821_CR_C2OUTPUT && MC6821_CR_C2ENABLEIRQ))
					== MC6821_CR_C2ENABLEIRQ)
					setControlRegisterA(controlRegisterA | MC6821_CR_IRQ2FLAG);
			}
			if ((controlRegisterA & MC6821_CR_C2OUTPUT) && (ca2 != value))
				postNotification(MC6821_CA2_CHANGED, NULL);
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
			bool isLowToHigh = (controlRegisterB & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && cb1 && !value) ||
				(isLowToHigh && !cb1 && value))
			{
				if (controlRegisterB & MC6821_CR_C1ENABLEIRQ)
					setControlRegisterB(controlRegisterB | MC6821_CR_IRQ1FLAG);
				if ((controlRegisterB & 
					 (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE))
					 == MC6821_CR_C2OUTPUT)
				{
					int value = 0;
					ioctl(MC6821_SET_CB2, &value);
				}
			}
			cb1 = value;
			
			return true;
		}
		case MC6821_SET_CB2:
		{
			bool value = *((int *) data);
			bool isLowToHigh = (controlRegisterB & MC6821_CR_C1LOWTOHIGH);
			if ((!isLowToHigh && cb1 && !value) ||
				(isLowToHigh && !cb1 && value))
			{
				if ((controlRegisterB & (MC6821_CR_C2OUTPUT && MC6821_CR_C2ENABLEIRQ))
					== MC6821_CR_C1ENABLEIRQ)
					setControlRegisterB(controlRegisterB | MC6821_CR_IRQ2FLAG);
			}
			if (cb2 != value)
				postNotification(MC6821_CB2_CHANGED, NULL);
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
	
	return false;
}

OEUInt8 MC6821::read(int address)
{
	switch(address & 0x3)
	{
		case MC6821_RS_DATAREGISTERA:
			if (controlRegisterA & MC6821_CR_DATAREGISTER)
			{
				if (controlRegisterA & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
					== MC6821_CR_C2OUTPUT)
				{
					int ca2 = 0;
					ioctl(MC6821_SET_CA2, &ca2);
					if (controlRegisterA & MC6821_CR_C2ERESTORE)
					{
						ca2 = 1;
						ioctl(MC6821_SET_CA2, &ca2);
					}
				}
				setControlRegisterA(controlRegisterA & 
									~(MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG));
				dataRegisterA &= dataDirectionRegisterA;
				dataRegisterA |= interfaceA->read(0) & ~dataDirectionRegisterA;
				return dataRegisterA;
			}
			else
				return dataDirectionRegisterA;
		case MC6821_RS_CONTROLREGISTERA:
			return controlRegisterA;
		case MC6821_RS_DATAREGISTERB:
			if (controlRegisterB & MC6821_CR_DATAREGISTER)
			{
				setControlRegisterB(controlRegisterB & 
									~(MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG));
				dataRegisterB &= dataDirectionRegisterB;
				dataRegisterB |= interfaceA->read(0) & ~dataDirectionRegisterB;
				return dataRegisterB;
			}
			else
				return dataDirectionRegisterB;
		case MC6821_RS_CONTROLREGISTERB:
			return controlRegisterB;
	}
	
	return 0;
}

void MC6821::write(int address, OEUInt8 value)
{
	switch(address & 0x3)
	{
		case MC6821_RS_DATAREGISTERA:
			if (controlRegisterA & MC6821_CR_DATAREGISTER)
			{
				dataRegisterA = value;
				interfaceA->write(0, value);
			}
			else
				dataDirectionRegisterA = value;
			break;
		case MC6821_RS_CONTROLREGISTERA:
			setControlRegisterA(value);
			if (value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT) ==
				(MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
			{
				int ca2 = value & MC6821_CR_C2SET;
				ioctl(MC6821_SET_CA2, &ca2);
			}
			break;
		case MC6821_RS_DATAREGISTERB:
			if (controlRegisterB & MC6821_CR_DATAREGISTER)
			{
				if (controlRegisterB & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
					== MC6821_CR_C2OUTPUT)
				{
					int cb2 = 0;
					ioctl(MC6821_SET_CB2, &cb2);
					if (controlRegisterB & MC6821_CR_C2ERESTORE)
					{
						cb2 = 1;
						ioctl(MC6821_SET_CB2, &cb2);
					}
				}
				dataRegisterB = value;
				interfaceB->write(0, value);
			}
			else
				dataDirectionRegisterB = value;
			break;
		case MC6821_RS_CONTROLREGISTERB:
			setControlRegisterB(value);
			if (value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)
				== (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
			{
				int cb2 = value & MC6821_CR_C2SET;
				ioctl(MC6821_SET_CB2, &cb2);
			}
			break;
	}
}
