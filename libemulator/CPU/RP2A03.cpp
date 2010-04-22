
/**
 * libemulator
 * RP2A03
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a Ricoh 2A03 microprocessor.
 */

#include "RP2A03.h"

#include "MOS6502Opcodes.h"
#include "RP2A03Opcodes.h"

RP2A03::RP2A03()
{
	sp.b.h = 0x01;
}

int RP2A03::ioctl(int message, void *data)
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
				a = getInt(property->value);
			else if (property->name == "x")
				x = getInt(property->value);
			else if (property->name == "y")
				y = getInt(property->value);
			else if (property->name == "p")
				p = getInt(property->value);
			else if (property->name == "sp")
				sp.b.l = getInt(property->value);
			else if (property->name == "pc")
				pc.w.l = getInt(property->value);
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

void RP2A03::execute()
{
	int opcode;
	
	switch (opcode)
	{
		MOS6502_OP(00);
		MOS6502_OP(20);
		MOS6502_OP(40);
		MOS6502_OP(60);
		MOS6502_OP(80);
		MOS6502_OP(a0);
		MOS6502_OP(c0);
		MOS6502_OP(e0);
		
		MOS6502_OP(10);
		MOS6502_OP(30);
		MOS6502_OP(50);
		MOS6502_OP(70);
		MOS6502_OP(90);
		MOS6502_OP(b0);
		MOS6502_OP(d0);
		MOS6502_OP(f0);
		
		MOS6502_OP(01);
		MOS6502_OP(21);
		MOS6502_OP(41);
		RP2A03_OP(61);
		MOS6502_OP(81);
		MOS6502_OP(a1);
		MOS6502_OP(c1);
		RP2A03_OP(e1);
		
		MOS6502_OP(11);
		MOS6502_OP(31);
		MOS6502_OP(51);
		RP2A03_OP(71);
		MOS6502_OP(91);
		MOS6502_OP(b1);
		MOS6502_OP(d1);
		RP2A03_OP(f1);
		
		MOS6502_OP(02);
		MOS6502_OP(22);
		MOS6502_OP(42);
		MOS6502_OP(62);
		MOS6502_OP(82);
		MOS6502_OP(a2);
		MOS6502_OP(c2);
		MOS6502_OP(e2);
		
		MOS6502_OP(12);
		MOS6502_OP(32);
		MOS6502_OP(52);
		MOS6502_OP(72);
		MOS6502_OP(92);
		MOS6502_OP(b2);
		MOS6502_OP(d2);
		MOS6502_OP(f2);
		
		MOS6502_OP(03);
		MOS6502_OP(23);
		MOS6502_OP(43);
		RP2A03_OP(63);
		MOS6502_OP(83);
		MOS6502_OP(a3);
		MOS6502_OP(c3);
		RP2A03_OP(e3);
		
		MOS6502_OP(13);
		MOS6502_OP(33);
		MOS6502_OP(53);
		RP2A03_OP(73);
		MOS6502_OP(93);
		MOS6502_OP(b3);
		MOS6502_OP(d3);
		RP2A03_OP(f3);
		
		MOS6502_OP(04);
		MOS6502_OP(24);
		MOS6502_OP(44);
		MOS6502_OP(64);
		MOS6502_OP(84);
		MOS6502_OP(a4);
		MOS6502_OP(c4);
		MOS6502_OP(e4);
		
		MOS6502_OP(14);
		MOS6502_OP(34);
		MOS6502_OP(54);
		MOS6502_OP(74);
		MOS6502_OP(94);
		MOS6502_OP(b4);
		MOS6502_OP(d4);
		MOS6502_OP(f4);
		
		MOS6502_OP(05);
		MOS6502_OP(25);
		MOS6502_OP(45);
		RP2A03_OP(65);
		MOS6502_OP(85);
		MOS6502_OP(a5);
		MOS6502_OP(c5);
		RP2A03_OP(e5);
		
		MOS6502_OP(15);
		MOS6502_OP(35);
		MOS6502_OP(55);
		RP2A03_OP(75);
		MOS6502_OP(95);
		MOS6502_OP(b5);
		MOS6502_OP(d5);
		RP2A03_OP(f5);
		
		MOS6502_OP(06);
		MOS6502_OP(26);
		MOS6502_OP(46);
		MOS6502_OP(66);
		MOS6502_OP(86);
		MOS6502_OP(a6);
		MOS6502_OP(c6);
		MOS6502_OP(e6);
		
		MOS6502_OP(16);
		MOS6502_OP(36);
		MOS6502_OP(56);
		MOS6502_OP(76);
		MOS6502_OP(96);
		MOS6502_OP(b6);
		MOS6502_OP(d6);
		MOS6502_OP(f6);
		
		MOS6502_OP(07);
		MOS6502_OP(27);
		MOS6502_OP(47);
		RP2A03_OP(67);
		MOS6502_OP(87);
		MOS6502_OP(a7);
		MOS6502_OP(c7);
		RP2A03_OP(e7);
		
		MOS6502_OP(17);
		MOS6502_OP(37);
		MOS6502_OP(57);
		RP2A03_OP(77);
		MOS6502_OP(97);
		MOS6502_OP(b7);
		MOS6502_OP(d7);
		RP2A03_OP(f7);
		
		MOS6502_OP(08);
		MOS6502_OP(28);
		MOS6502_OP(48);
		MOS6502_OP(68);
		MOS6502_OP(88);
		MOS6502_OP(a8);
		MOS6502_OP(c8);
		MOS6502_OP(e8);
		
		MOS6502_OP(18);
		MOS6502_OP(38);
		MOS6502_OP(58);
		MOS6502_OP(78);
		MOS6502_OP(98);
		MOS6502_OP(b8);
		MOS6502_OP(d8);
		MOS6502_OP(f8);
		
		MOS6502_OP(09);
		MOS6502_OP(29);
		MOS6502_OP(49);
		RP2A03_OP(69);
		MOS6502_OP(89);
		MOS6502_OP(a9);
		MOS6502_OP(c9);
		RP2A03_OP(e9);
		
		MOS6502_OP(19);
		MOS6502_OP(39);
		MOS6502_OP(59);
		RP2A03_OP(79);
		MOS6502_OP(99);
		MOS6502_OP(b9);
		MOS6502_OP(d9);
		RP2A03_OP(f9);
		
		MOS6502_OP(0a);
		MOS6502_OP(2a);
		MOS6502_OP(4a);
		MOS6502_OP(6a);
		MOS6502_OP(8a);
		MOS6502_OP(aa);
		MOS6502_OP(ca);
		MOS6502_OP(ea);
		
		MOS6502_OP(1a);
		MOS6502_OP(3a);
		MOS6502_OP(5a);
		MOS6502_OP(7a);
		MOS6502_OP(9a);
		MOS6502_OP(ba);
		MOS6502_OP(da);
		MOS6502_OP(fa);
		
		MOS6502_OP(0b);
		MOS6502_OP(2b);
		MOS6502_OP(4b);
		RP2A03_OP(6b);
		MOS6502_OP(8b);
		RP2A03_OP(ab);
		MOS6502_OP(cb);
		RP2A03_OP(eb);
		
		MOS6502_OP(1b);
		MOS6502_OP(3b);
		MOS6502_OP(5b);
		RP2A03_OP(7b);
		MOS6502_OP(9b);
		MOS6502_OP(bb);
		MOS6502_OP(db);
		RP2A03_OP(fb);
		
		MOS6502_OP(0c);
		MOS6502_OP(2c);
		MOS6502_OP(4c);
		MOS6502_OP(6c);
		MOS6502_OP(8c);
		MOS6502_OP(ac);
		MOS6502_OP(cc);
		MOS6502_OP(ec);
		
		MOS6502_OP(1c);
		MOS6502_OP(3c);
		MOS6502_OP(5c);
		MOS6502_OP(7c);
		RP2A03_OP(9c);
		MOS6502_OP(bc);
		MOS6502_OP(dc);
		MOS6502_OP(fc);
		
		MOS6502_OP(0d);
		MOS6502_OP(2d);
		MOS6502_OP(4d);
		RP2A03_OP(6d);
		MOS6502_OP(8d);
		MOS6502_OP(ad);
		MOS6502_OP(cd);
		RP2A03_OP(ed);
		
		MOS6502_OP(1d);
		MOS6502_OP(3d);
		MOS6502_OP(5d);
		RP2A03_OP(7d);
		MOS6502_OP(9d);
		MOS6502_OP(bd);
		MOS6502_OP(dd);
		RP2A03_OP(fd);
		
		MOS6502_OP(0e);
		MOS6502_OP(2e);
		MOS6502_OP(4e);
		MOS6502_OP(6e);
		MOS6502_OP(8e);
		MOS6502_OP(ae);
		MOS6502_OP(ce);
		MOS6502_OP(ee);
		
		MOS6502_OP(1e);
		MOS6502_OP(3e);
		MOS6502_OP(5e);
		MOS6502_OP(7e);
		RP2A03_OP(9e);
		MOS6502_OP(be);
		MOS6502_OP(de);
		MOS6502_OP(fe);
		
		MOS6502_OP(0f);
		MOS6502_OP(2f);
		MOS6502_OP(4f);
		RP2A03_OP(6f);
		MOS6502_OP(8f);
		MOS6502_OP(af);
		MOS6502_OP(cf);
		RP2A03_OP(ef);
		
		MOS6502_OP(1f);
		MOS6502_OP(3f);
		MOS6502_OP(5f);
		RP2A03_OP(7f);
		MOS6502_OP(9f);
		MOS6502_OP(bf);
		MOS6502_OP(df);
		RP2A03_OP(ff);
	}
}
