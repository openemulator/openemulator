
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

#include "MOS6502Operations.h"
#include "MOS6502IllegalOperations.h"
#include "MOS6502Opcodes.h"

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

void MOS6502::execute()
{
	int opcode;
	
	switch (opcode)
	{
		M6502_OP(00);
		M6502_OP(20);
		M6502_OP(40);
		M6502_OP(60);
		M6502_OP(80);
		M6502_OP(a0);
		M6502_OP(c0);
		M6502_OP(e0);
		
		M6502_OP(10);
		M6502_OP(30);
		M6502_OP(50);
		M6502_OP(70);
		M6502_OP(90);
		M6502_OP(b0);
		M6502_OP(d0);
		M6502_OP(f0);
		
		M6502_OP(01);
		M6502_OP(21);
		M6502_OP(41);
		M6502_OP(61);
		M6502_OP(81);
		M6502_OP(a1);
		M6502_OP(c1);
		M6502_OP(e1);
			
		M6502_OP(11);
		M6502_OP(31);
		M6502_OP(51);
		M6502_OP(71);
		M6502_OP(91);
		M6502_OP(b1);
		M6502_OP(d1);
		M6502_OP(f1);
			
		M6502_OP(02);
		M6502_OP(22);
		M6502_OP(42);
		M6502_OP(62);
		M6502_OP(82);
		M6502_OP(a2);
		M6502_OP(c2);
		M6502_OP(e2);
			
		M6502_OP(12);
		M6502_OP(32);
		M6502_OP(52);
		M6502_OP(72);
		M6502_OP(92);
		M6502_OP(b2);
		M6502_OP(d2);
		M6502_OP(f2);
			
		M6502_OP(03);
		M6502_OP(23);
		M6502_OP(43);
		M6502_OP(63);
		M6502_OP(83);
		M6502_OP(a3);
		M6502_OP(c3);
		M6502_OP(e3);
			
		M6502_OP(13);
		M6502_OP(33);
		M6502_OP(53);
		M6502_OP(73);
		M6502_OP(93);
		M6502_OP(b3);
		M6502_OP(d3);
		M6502_OP(f3);
		
		M6502_OP(04);
		M6502_OP(24);
		M6502_OP(44);
		M6502_OP(64);
		M6502_OP(84);
		M6502_OP(a4);
		M6502_OP(c4);
		M6502_OP(e4);
			
		M6502_OP(14);
		M6502_OP(34);
		M6502_OP(54);
		M6502_OP(74);
		M6502_OP(94);
		M6502_OP(b4);
		M6502_OP(d4);
		M6502_OP(f4);
			
		M6502_OP(05);
		M6502_OP(25);
		M6502_OP(45);
		M6502_OP(65);
		M6502_OP(85);
		M6502_OP(a5);
		M6502_OP(c5);
		M6502_OP(e5);
			
		M6502_OP(15);
		M6502_OP(35);
		M6502_OP(55);
		M6502_OP(75);
		M6502_OP(95);
		M6502_OP(b5);
		M6502_OP(d5);
		M6502_OP(f5);
			
		M6502_OP(06);
		M6502_OP(26);
		M6502_OP(46);
		M6502_OP(66);
		M6502_OP(86);
		M6502_OP(a6);
		M6502_OP(c6);
		M6502_OP(e6);
			
		M6502_OP(16);
		M6502_OP(36);
		M6502_OP(56);
		M6502_OP(76);
		M6502_OP(96);
		M6502_OP(b6);
		M6502_OP(d6);
		M6502_OP(f6);
			
		M6502_OP(07);
		M6502_OP(27);
		M6502_OP(47);
		M6502_OP(67);
		M6502_OP(87);
		M6502_OP(a7);
		M6502_OP(c7);
		M6502_OP(e7);
			
		M6502_OP(17);
		M6502_OP(37);
		M6502_OP(57);
		M6502_OP(77);
		M6502_OP(97);
		M6502_OP(b7);
		M6502_OP(d7);
		M6502_OP(f7);
			
		M6502_OP(08);
		M6502_OP(28);
		M6502_OP(48);
		M6502_OP(68);
		M6502_OP(88);
		M6502_OP(a8);
		M6502_OP(c8);
		M6502_OP(e8);
			
		M6502_OP(18);
		M6502_OP(38);
		M6502_OP(58);
		M6502_OP(78);
		M6502_OP(98);
		M6502_OP(b8);
		M6502_OP(d8);
		M6502_OP(f8);
			
		M6502_OP(09);
		M6502_OP(29);
		M6502_OP(49);
		M6502_OP(69);
		M6502_OP(89);
		M6502_OP(a9);
		M6502_OP(c9);
		M6502_OP(e9);
			
		M6502_OP(19);
		M6502_OP(39);
		M6502_OP(59);
		M6502_OP(79);
		M6502_OP(99);
		M6502_OP(b9);
		M6502_OP(d9);
		M6502_OP(f9);
			
		M6502_OP(0a);
		M6502_OP(2a);
		M6502_OP(4a);
		M6502_OP(6a);
		M6502_OP(8a);
		M6502_OP(aa);
		M6502_OP(ca);
		M6502_OP(ea);
			
		M6502_OP(1a);
		M6502_OP(3a);
		M6502_OP(5a);
		M6502_OP(7a);
		M6502_OP(9a);
		M6502_OP(ba);
		M6502_OP(da);
		M6502_OP(fa);
			
		M6502_OP(0b);
		M6502_OP(2b);
		M6502_OP(4b);
		M6502_OP(6b);
		M6502_OP(8b);
		M6502_OP(ab);
		M6502_OP(cb);
		M6502_OP(eb);
			
		M6502_OP(1b);
		M6502_OP(3b);
		M6502_OP(5b);
		M6502_OP(7b);
		M6502_OP(9b);
		M6502_OP(bb);
		M6502_OP(db);
		M6502_OP(fb);
			
		M6502_OP(0c);
		M6502_OP(2c);
		M6502_OP(4c);
		M6502_OP(6c);
		M6502_OP(8c);
		M6502_OP(ac);
		M6502_OP(cc);
		M6502_OP(ec);
			
		M6502_OP(1c);
		M6502_OP(3c);
		M6502_OP(5c);
		M6502_OP(7c);
		M6502_OP(9c);
		M6502_OP(bc);
		M6502_OP(dc);
		M6502_OP(fc);
			
		M6502_OP(0d);
		M6502_OP(2d);
		M6502_OP(4d);
		M6502_OP(6d);
		M6502_OP(8d);
		M6502_OP(ad);
		M6502_OP(cd);
		M6502_OP(ed);
			
		M6502_OP(1d);
		M6502_OP(3d);
		M6502_OP(5d);
		M6502_OP(7d);
		M6502_OP(9d);
		M6502_OP(bd);
		M6502_OP(dd);
		M6502_OP(fd);
			
		M6502_OP(0e);
		M6502_OP(2e);
		M6502_OP(4e);
		M6502_OP(6e);
		M6502_OP(8e);
		M6502_OP(ae);
		M6502_OP(ce);
		M6502_OP(ee);
			
		M6502_OP(1e);
		M6502_OP(3e);
		M6502_OP(5e);
		M6502_OP(7e);
		M6502_OP(9e);
		M6502_OP(be);
		M6502_OP(de);
		M6502_OP(fe);
			
		M6502_OP(0f);
		M6502_OP(2f);
		M6502_OP(4f);
		M6502_OP(6f);
		M6502_OP(8f);
		M6502_OP(af);
		M6502_OP(cf);
		M6502_OP(ef);
			
		M6502_OP(1f);
		M6502_OP(3f);
		M6502_OP(5f);
		M6502_OP(7f);
		M6502_OP(9f);
		M6502_OP(bf);
		M6502_OP(df);
		M6502_OP(ff);
	}
}