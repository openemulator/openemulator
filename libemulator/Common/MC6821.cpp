
/**
 * libemulator
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
	irqA = NULL;
	interfaceB = NULL;
	irqB = NULL;
	
	reset();
}

void MC6821::reset()
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

void MC6821::setControlRegisterA(int value)
{
	bool wasIRQ = controlRegisterA & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	controlRegisterA = value;
	bool isIRQ = controlRegisterA & (MC6821_CR_IRQ1FLAG | MC6821_CR_IRQ2FLAG);
	
	if (irqA)
	{
		if (wasIRQ && !isIRQ)
			irqA->ioctl(OEIOCTL_RELEASE_IRQ, NULL);
		else if (!wasIRQ && isIRQ)
			irqA->ioctl(OEIOCTL_ASSERT_IRQ, NULL);
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
			irqB->ioctl(OEIOCTL_RELEASE_IRQ, NULL);
		else if (!wasIRQ && isIRQ)
			irqB->ioctl(OEIOCTL_ASSERT_IRQ, NULL);
	}
}

int MC6821::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "controlRegisterA")
				setControlRegisterA(getInt(property->value));
			else if (property->name == "dataDirectionRegisterA")
				dataDirectionRegisterA = getInt(property->value);
			else if (property->name == "dataRegisterA")
				dataRegisterA = getInt(property->value);
			else if (property->name == "controlRegisterB")
				setControlRegisterB(getInt(property->value));
			else if (property->name == "dataDirectionRegisterB")
				dataDirectionRegisterB = getInt(property->value);
			else if (property->name == "dataRegisterB")
				dataRegisterB = getInt(property->value);
			else if (property->name == "ca1")
				ca1 = getInt(property->value);
			else if (property->name == "ca2")
				ca2 = getInt(property->value);
			else if (property->name == "cb1")
				cb1 = getInt(property->value);
			else if (property->name == "cb2")
				cb2 = getInt(property->value);
			
			break;
		}
		case OEIOCTL_GET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "controlRegisterA")
				property->value = getHex(controlRegisterA);
			else if (property->name == "dataDirectionRegisterA")
				property->value = getHex(dataDirectionRegisterA);
			else if (property->name == "dataRegisterA")
				property->value = getHex(dataRegisterA);
			else if (property->name == "controlRegisterB")
				property->value = getHex(controlRegisterB);
			else if (property->name == "dataDirectionRegisterB")
				property->value = getHex(dataDirectionRegisterB);
			else if (property->name == "dataRegisterB")
				property->value = getHex(dataRegisterB);
			else if (property->name == "ca1")
				property->value = ca1;
			else if (property->name == "ca2")
				property->value = ca2;
			else if (property->name == "cb1")
				property->value = cb1;
			else if (property->name == "cb2")
				property->value = cb2;
			else
				return false;
			
			return true;
		}
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "interfaceA")
				interfaceA = connection->component;
			else if (connection->name == "interfaceB")
				interfaceB = connection->component;
			else if (connection->name == "irqA")
				irqA = connection->component;
			else if (connection->name == "irqB")
				irqB = connection->component;
			
			break;
		}
		case MC6821_RESET:
		{
			reset();
			break;
		}
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
			
			break;
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
			
			break;
		}
		case MC6821_GET_CA2:
		{
			int *value = (int *) data;
			*value = ca2;
			
			break;
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
			
			break;
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
			
			break;
		}
		case MC6821_GET_CB2:
		{
			int *value = (int *) data;
			*value = cb2;
			
			break;
		}
	}
	
	return false;
}

int MC6821::read(int address)
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

void MC6821::write(int address, int value)
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
