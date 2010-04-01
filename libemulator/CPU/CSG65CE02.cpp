
/**
 * libemulator
 * CSG65CE02
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a California Semiconductor Group 65CE02 microprocessor.
 */

#include "CSG65CE02.h"

#include "MOS6502Operations.h"
#include "MOS6502IllegalOperations.h"
#include "CSG65CE02Operations.h"
#include "CSG65CE02Opcodes.h"

CSG65CE02::CSG65CE02()
{
	sp.b.h = 0x01;
}

int CSG65CE02::ioctl(int message, void *data)
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

void CSG65CE02::execute()
{
	int opcode;
	
	switch (opcode)
	{
			W65C02_OP(00);
			W65C02_OP(20);
			W65C02_OP(40);
			W65C02_OP(60);
			W65C02_OP(80);
			W65C02_OP(a0);
			W65C02_OP(c0);
			W65C02_OP(e0);
			
			W65C02_OP(10);
			W65C02_OP(30);
			W65C02_OP(50);
			W65C02_OP(70);
			W65C02_OP(90);
			W65C02_OP(b0);
			W65C02_OP(d0);
			W65C02_OP(f0);
			
			W65C02_OP(01);
			W65C02_OP(21);
			W65C02_OP(41);
			W65C02_OP(61);
			W65C02_OP(81);
			W65C02_OP(a1);
			W65C02_OP(c1);
			W65C02_OP(e1);
			
			W65C02_OP(11);
			W65C02_OP(31);
			W65C02_OP(51);
			W65C02_OP(71);
			W65C02_OP(91);
			W65C02_OP(b1);
			W65C02_OP(d1);
			W65C02_OP(f1);
			
			W65C02_OP(02);
			W65C02_OP(22);
			W65C02_OP(42);
			W65C02_OP(62);
			W65C02_OP(82);
			W65C02_OP(a2);
			W65C02_OP(c2);
			W65C02_OP(e2);
			
			W65C02_OP(12);
			W65C02_OP(32);
			W65C02_OP(52);
			W65C02_OP(72);
			W65C02_OP(92);
			W65C02_OP(b2);
			W65C02_OP(d2);
			W65C02_OP(f2);
			
			W65C02_OP(03);
			W65C02_OP(23);
			W65C02_OP(43);
			W65C02_OP(63);
			W65C02_OP(83);
			W65C02_OP(a3);
			W65C02_OP(c3);
			W65C02_OP(e3);
			
			W65C02_OP(13);
			W65C02_OP(33);
			W65C02_OP(53);
			W65C02_OP(73);
			W65C02_OP(93);
			W65C02_OP(b3);
			W65C02_OP(d3);
			W65C02_OP(f3);
			
			W65C02_OP(04);
			W65C02_OP(24);
			W65C02_OP(44);
			W65C02_OP(64);
			W65C02_OP(84);
			W65C02_OP(a4);
			W65C02_OP(c4);
			W65C02_OP(e4);
			
			W65C02_OP(14);
			W65C02_OP(34);
			W65C02_OP(54);
			W65C02_OP(74);
			W65C02_OP(94);
			W65C02_OP(b4);
			W65C02_OP(d4);
			W65C02_OP(f4);
			
			W65C02_OP(05);
			W65C02_OP(25);
			W65C02_OP(45);
			W65C02_OP(65);
			W65C02_OP(85);
			W65C02_OP(a5);
			W65C02_OP(c5);
			W65C02_OP(e5);
			
			W65C02_OP(15);
			W65C02_OP(35);
			W65C02_OP(55);
			W65C02_OP(75);
			W65C02_OP(95);
			W65C02_OP(b5);
			W65C02_OP(d5);
			W65C02_OP(f5);
			
			W65C02_OP(06);
			W65C02_OP(26);
			W65C02_OP(46);
			W65C02_OP(66);
			W65C02_OP(86);
			W65C02_OP(a6);
			W65C02_OP(c6);
			W65C02_OP(e6);
			
			W65C02_OP(16);
			W65C02_OP(36);
			W65C02_OP(56);
			W65C02_OP(76);
			W65C02_OP(96);
			W65C02_OP(b6);
			W65C02_OP(d6);
			W65C02_OP(f6);
			
			W65C02_OP(07);
			W65C02_OP(27);
			W65C02_OP(47);
			W65C02_OP(67);
			W65C02_OP(87);
			W65C02_OP(a7);
			W65C02_OP(c7);
			W65C02_OP(e7);
			
			W65C02_OP(17);
			W65C02_OP(37);
			W65C02_OP(57);
			W65C02_OP(77);
			W65C02_OP(97);
			W65C02_OP(b7);
			W65C02_OP(d7);
			W65C02_OP(f7);
			
			W65C02_OP(08);
			W65C02_OP(28);
			W65C02_OP(48);
			W65C02_OP(68);
			W65C02_OP(88);
			W65C02_OP(a8);
			W65C02_OP(c8);
			W65C02_OP(e8);
			
			W65C02_OP(18);
			W65C02_OP(38);
			W65C02_OP(58);
			W65C02_OP(78);
			W65C02_OP(98);
			W65C02_OP(b8);
			W65C02_OP(d8);
			W65C02_OP(f8);
			
			W65C02_OP(09);
			W65C02_OP(29);
			W65C02_OP(49);
			W65C02_OP(69);
			W65C02_OP(89);
			W65C02_OP(a9);
			W65C02_OP(c9);
			W65C02_OP(e9);
			
			W65C02_OP(19);
			W65C02_OP(39);
			W65C02_OP(59);
			W65C02_OP(79);
			W65C02_OP(99);
			W65C02_OP(b9);
			W65C02_OP(d9);
			W65C02_OP(f9);
			
			W65C02_OP(0a);
			W65C02_OP(2a);
			W65C02_OP(4a);
			W65C02_OP(6a);
			W65C02_OP(8a);
			W65C02_OP(aa);
			W65C02_OP(ca);
			W65C02_OP(ea);
			
			W65C02_OP(1a);
			W65C02_OP(3a);
			W65C02_OP(5a);
			W65C02_OP(7a);
			W65C02_OP(9a);
			W65C02_OP(ba);
			W65C02_OP(da);
			W65C02_OP(fa);
			
			W65C02_OP(0b);
			W65C02_OP(2b);
			W65C02_OP(4b);
			W65C02_OP(6b);
			W65C02_OP(8b);
			W65C02_OP(ab);
			W65C02_OP(cb);
			W65C02_OP(eb);
			
			W65C02_OP(1b);
			W65C02_OP(3b);
			W65C02_OP(5b);
			W65C02_OP(7b);
			W65C02_OP(9b);
			W65C02_OP(bb);
			W65C02_OP(db);
			W65C02_OP(fb);
			
			W65C02_OP(0c);
			W65C02_OP(2c);
			W65C02_OP(4c);
			W65C02_OP(6c);
			W65C02_OP(8c);
			W65C02_OP(ac);
			W65C02_OP(cc);
			W65C02_OP(ec);
			
			W65C02_OP(1c);
			W65C02_OP(3c);
			W65C02_OP(5c);
			W65C02_OP(7c);
			W65C02_OP(9c);
			W65C02_OP(bc);
			W65C02_OP(dc);
			W65C02_OP(fc);
			
			W65C02_OP(0d);
			W65C02_OP(2d);
			W65C02_OP(4d);
			W65C02_OP(6d);
			W65C02_OP(8d);
			W65C02_OP(ad);
			W65C02_OP(cd);
			W65C02_OP(ed);
			
			W65C02_OP(1d);
			W65C02_OP(3d);
			W65C02_OP(5d);
			W65C02_OP(7d);
			W65C02_OP(9d);
			W65C02_OP(bd);
			W65C02_OP(dd);
			W65C02_OP(fd);
			
			W65C02_OP(0e);
			W65C02_OP(2e);
			W65C02_OP(4e);
			W65C02_OP(6e);
			W65C02_OP(8e);
			W65C02_OP(ae);
			W65C02_OP(ce);
			W65C02_OP(ee);
			
			W65C02_OP(1e);
			W65C02_OP(3e);
			W65C02_OP(5e);
			W65C02_OP(7e);
			W65C02_OP(9e);
			W65C02_OP(be);
			W65C02_OP(de);
			W65C02_OP(fe);
			
			W65C02_OP(0f);
			W65C02_OP(2f);
			W65C02_OP(4f);
			W65C02_OP(6f);
			W65C02_OP(8f);
			W65C02_OP(af);
			W65C02_OP(cf);
			W65C02_OP(ef);
			
			W65C02_OP(1f);
			W65C02_OP(3f);
			W65C02_OP(5f);
			W65C02_OP(7f);
			W65C02_OP(9f);
			W65C02_OP(bf);
			W65C02_OP(df);
			W65C02_OP(ff);
	}
}