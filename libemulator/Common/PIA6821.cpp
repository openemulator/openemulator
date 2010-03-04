
/**
 * libemulator
 * PIA6821
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 6821 Peripheral Interface Adapter
 */

#include "PIA6821.h"

PIA6821::PIA6821()
{
	reset();
}

void PIA6821::reset()
{
	controlRegisterA = 0;
	dataDirectionRegisterA = 0;
	dataRegisterA = 0;
	controlRegisterB = 0;
	dataDirectionRegisterB = 0;
	dataRegisterB = 0;
}

int PIA6821::ioctl(int message, void *data)
{
	switch(message)
	{
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
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
			else if (property->name == "offset")
				size = intValue(property->value);
			break;
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlRange *range = (OEIoctlRange *) data;
			range->offset = offset;
			range->size = size;
			break;
		}
		case PIA6821_RESET:
		{
			reset();
			break;
		}
	}
	
	return 0;
}

int PIA6821::read(int address)
{
	switch(address & 0x3)
	{
		case PIA6821_RS_DATAREGISTERA:
			if (controlRegisterA & PIA6821_CR_DATAREGISTER)
			{
				if (controlRegisterA)
					break;
				return dataRegisterA;
			}
			else
				return dataDirectionRegisterA;
		case PIA6821_RS_CONTROLREGISTERA:
			return controlRegisterA;
		case PIA6821_RS_DATAREGISTERB:
			if (controlRegisterB & PIA6821_CR_DATAREGISTER)
				return dataRegisterB;
			else
				return dataDirectionRegisterB;
		case PIA6821_RS_CONTROLREGISTERB:
			return controlRegisterB;
	}
	
	return 0;
}

void PIA6821::write(int address, int value)
{
	switch(address & 0x3)
	{
		case PIA6821_RS_DATAREGISTERA:
		case PIA6821_RS_CONTROLREGISTERA:
		case PIA6821_RS_DATAREGISTERB:
		case PIA6821_RS_CONTROLREGISTERB:
			break;
	}
}
