
/**
 * libemulator
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a MOS6502 microprocessor.
 */

#include "MOS6502.h"

int MOS6502::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "memoryMap")
				memoryMap = connection->component;
			else if (connection->name == "hostSystem")
				connection->component->addObserver(this);
			break;
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "a")
				a = intValue(property->value);
			else if (property->name == "x")
				x = intValue(property->value);
			else if (property->name == "y")
				y = intValue(property->value);
			else if (property->name == "s")
				s = intValue(property->value);
			else if (property->name == "p")
				p = intValue(property->value);
			else if (property->name == "pc")
				pc = intValue(property->value);
			break;
		}
		case OEIOCTL_GET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "a")
				property->value = a;
			else if (property->name == "x")
				property->value = x;
			else if (property->name == "y")
				property->value = y;
			else if (property->name == "s")
				property->value = s;
			else if (property->name == "p")
				property->value = p;
			else if (property->name == "pc")
				property->value = pc;
			else
				return false;
			
			return true;
		}
	}
	return false;
}
