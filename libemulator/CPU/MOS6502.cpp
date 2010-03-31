
/**
 * libemulator
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a MOS6502 microprocessor.
 */

#include "MOS6502.h"

#define M6502_NMI_VEC	0xfffa
#define M6502_RST_VEC	0xfffc
#define M6502_IRQ_VEC	0xfffe

#include "MOS6502Opcodes.h"
#include "MOS6502IllegalOpcodes.h"
#include "MOS6502Impl.h"

MOS6502::MOS6502()
{
	sp.b.h = 0x01;
}

int MOS6502::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "memory")
				memory = connection->component;
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
			else if (property->name == "p")
				p = intValue(property->value);
			else if (property->name == "sp")
				sp.b.l = intValue(property->value);
			else if (property->name == "pc")
				pc.w.l = intValue(property->value);
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
			else if (property->name == "p")
				property->value = p;
			else if (property->name == "sp")
				property->value = sp.d;
			else if (property->name == "pc")
				property->value = pc.d;
			else
				return false;
			
			return true;
		}
	}
	return false;
}
