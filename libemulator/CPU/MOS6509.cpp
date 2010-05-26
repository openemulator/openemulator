
/**
 * libemulator
 * MOS6509
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6509 microprocessor.
 */

#include "MOS6509.h"
#include "MOS6509Opcodes.h"

MOS6509::MOS6509()
{
	sp.b.h = 0x01;
}

int MOS6509::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "memory")
				memory = connection->component;
//			else if (connection->name == "hostSystem")
//				connection->component->addObserver(this);
			break;
		}
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
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
		case OE_GET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
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

void MOS6509::execute()
{
	int opcode;
	
	switch (opcode)
	{
		MOS6509_OP(00);
		MOS6509_OP(20);
		MOS6509_OP(40);
		MOS6509_OP(60);
		MOS6509_OP(80);
		MOS6509_OP(a0);
		MOS6509_OP(c0);
		MOS6509_OP(e0);
		
		MOS6509_OP(10);
		MOS6509_OP(30);
		MOS6509_OP(50);
		MOS6509_OP(70);
		MOS6509_OP(90);
		MOS6509_OP(b0);
		MOS6509_OP(d0);
		MOS6509_OP(f0);
		
		MOS6509_OP(01);
		MOS6509_OP(21);
		MOS6509_OP(41);
		MOS6509_OP(61);
		MOS6509_OP(81);
		MOS6509_OP(a1);
		MOS6509_OP(c1);
		MOS6509_OP(e1);
			
		MOS6509_OP(11);
		MOS6509_OP(31);
		MOS6509_OP(51);
		MOS6509_OP(71);
		MOS6509_OP(91);
		MOS6509_OP(b1);
		MOS6509_OP(d1);
		MOS6509_OP(f1);
			
		MOS6509_OP(02);
		MOS6509_OP(22);
		MOS6509_OP(42);
		MOS6509_OP(62);
		MOS6509_OP(82);
		MOS6509_OP(a2);
		MOS6509_OP(c2);
		MOS6509_OP(e2);
			
		MOS6509_OP(12);
		MOS6509_OP(32);
		MOS6509_OP(52);
		MOS6509_OP(72);
		MOS6509_OP(92);
		MOS6509_OP(b2);
		MOS6509_OP(d2);
		MOS6509_OP(f2);
			
		MOS6509_OP(03);
		MOS6509_OP(23);
		MOS6509_OP(43);
		MOS6509_OP(63);
		MOS6509_OP(83);
		MOS6509_OP(a3);
		MOS6509_OP(c3);
		MOS6509_OP(e3);
			
		MOS6509_OP(13);
		MOS6509_OP(33);
		MOS6509_OP(53);
		MOS6509_OP(73);
		MOS6509_OP(93);
		MOS6509_OP(b3);
		MOS6509_OP(d3);
		MOS6509_OP(f3);
		
		MOS6509_OP(04);
		MOS6509_OP(24);
		MOS6509_OP(44);
		MOS6509_OP(64);
		MOS6509_OP(84);
		MOS6509_OP(a4);
		MOS6509_OP(c4);
		MOS6509_OP(e4);
			
		MOS6509_OP(14);
		MOS6509_OP(34);
		MOS6509_OP(54);
		MOS6509_OP(74);
		MOS6509_OP(94);
		MOS6509_OP(b4);
		MOS6509_OP(d4);
		MOS6509_OP(f4);
			
		MOS6509_OP(05);
		MOS6509_OP(25);
		MOS6509_OP(45);
		MOS6509_OP(65);
		MOS6509_OP(85);
		MOS6509_OP(a5);
		MOS6509_OP(c5);
		MOS6509_OP(e5);
			
		MOS6509_OP(15);
		MOS6509_OP(35);
		MOS6509_OP(55);
		MOS6509_OP(75);
		MOS6509_OP(95);
		MOS6509_OP(b5);
		MOS6509_OP(d5);
		MOS6509_OP(f5);
			
		MOS6509_OP(06);
		MOS6509_OP(26);
		MOS6509_OP(46);
		MOS6509_OP(66);
		MOS6509_OP(86);
		MOS6509_OP(a6);
		MOS6509_OP(c6);
		MOS6509_OP(e6);
			
		MOS6509_OP(16);
		MOS6509_OP(36);
		MOS6509_OP(56);
		MOS6509_OP(76);
		MOS6509_OP(96);
		MOS6509_OP(b6);
		MOS6509_OP(d6);
		MOS6509_OP(f6);
			
		MOS6509_OP(07);
		MOS6509_OP(27);
		MOS6509_OP(47);
		MOS6509_OP(67);
		MOS6509_OP(87);
		MOS6509_OP(a7);
		MOS6509_OP(c7);
		MOS6509_OP(e7);
			
		MOS6509_OP(17);
		MOS6509_OP(37);
		MOS6509_OP(57);
		MOS6509_OP(77);
		MOS6509_OP(97);
		MOS6509_OP(b7);
		MOS6509_OP(d7);
		MOS6509_OP(f7);
			
		MOS6509_OP(08);
		MOS6509_OP(28);
		MOS6509_OP(48);
		MOS6509_OP(68);
		MOS6509_OP(88);
		MOS6509_OP(a8);
		MOS6509_OP(c8);
		MOS6509_OP(e8);
			
		MOS6509_OP(18);
		MOS6509_OP(38);
		MOS6509_OP(58);
		MOS6509_OP(78);
		MOS6509_OP(98);
		MOS6509_OP(b8);
		MOS6509_OP(d8);
		MOS6509_OP(f8);
			
		MOS6509_OP(09);
		MOS6509_OP(29);
		MOS6509_OP(49);
		MOS6509_OP(69);
		MOS6509_OP(89);
		MOS6509_OP(a9);
		MOS6509_OP(c9);
		MOS6509_OP(e9);
			
		MOS6509_OP(19);
		MOS6509_OP(39);
		MOS6509_OP(59);
		MOS6509_OP(79);
		MOS6509_OP(99);
		MOS6509_OP(b9);
		MOS6509_OP(d9);
		MOS6509_OP(f9);
			
		MOS6509_OP(0a);
		MOS6509_OP(2a);
		MOS6509_OP(4a);
		MOS6509_OP(6a);
		MOS6509_OP(8a);
		MOS6509_OP(aa);
		MOS6509_OP(ca);
		MOS6509_OP(ea);
			
		MOS6509_OP(1a);
		MOS6509_OP(3a);
		MOS6509_OP(5a);
		MOS6509_OP(7a);
		MOS6509_OP(9a);
		MOS6509_OP(ba);
		MOS6509_OP(da);
		MOS6509_OP(fa);
			
		MOS6509_OP(0b);
		MOS6509_OP(2b);
		MOS6509_OP(4b);
		MOS6509_OP(6b);
		MOS6509_OP(8b);
		MOS6509_OP(ab);
		MOS6509_OP(cb);
		MOS6509_OP(eb);
			
		MOS6509_OP(1b);
		MOS6509_OP(3b);
		MOS6509_OP(5b);
		MOS6509_OP(7b);
		MOS6509_OP(9b);
		MOS6509_OP(bb);
		MOS6509_OP(db);
		MOS6509_OP(fb);
			
		MOS6509_OP(0c);
		MOS6509_OP(2c);
		MOS6509_OP(4c);
		MOS6509_OP(6c);
		MOS6509_OP(8c);
		MOS6509_OP(ac);
		MOS6509_OP(cc);
		MOS6509_OP(ec);
			
		MOS6509_OP(1c);
		MOS6509_OP(3c);
		MOS6509_OP(5c);
		MOS6509_OP(7c);
		MOS6509_OP(9c);
		MOS6509_OP(bc);
		MOS6509_OP(dc);
		MOS6509_OP(fc);
			
		MOS6509_OP(0d);
		MOS6509_OP(2d);
		MOS6509_OP(4d);
		MOS6509_OP(6d);
		MOS6509_OP(8d);
		MOS6509_OP(ad);
		MOS6509_OP(cd);
		MOS6509_OP(ed);
			
		MOS6509_OP(1d);
		MOS6509_OP(3d);
		MOS6509_OP(5d);
		MOS6509_OP(7d);
		MOS6509_OP(9d);
		MOS6509_OP(bd);
		MOS6509_OP(dd);
		MOS6509_OP(fd);
			
		MOS6509_OP(0e);
		MOS6509_OP(2e);
		MOS6509_OP(4e);
		MOS6509_OP(6e);
		MOS6509_OP(8e);
		MOS6509_OP(ae);
		MOS6509_OP(ce);
		MOS6509_OP(ee);
			
		MOS6509_OP(1e);
		MOS6509_OP(3e);
		MOS6509_OP(5e);
		MOS6509_OP(7e);
		MOS6509_OP(9e);
		MOS6509_OP(be);
		MOS6509_OP(de);
		MOS6509_OP(fe);
			
		MOS6509_OP(0f);
		MOS6509_OP(2f);
		MOS6509_OP(4f);
		MOS6509_OP(6f);
		MOS6509_OP(8f);
		MOS6509_OP(af);
		MOS6509_OP(cf);
		MOS6509_OP(ef);
			
		MOS6509_OP(1f);
		MOS6509_OP(3f);
		MOS6509_OP(5f);
		MOS6509_OP(7f);
		MOS6509_OP(9f);
		MOS6509_OP(bf);
		MOS6509_OP(df);
		MOS6509_OP(ff);
	}
}