
/**
 * libemulator
 * W65C02S
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a W65C02S microprocessor.
 */

#include "W65C02S.h"
#include "W65C02SOpcodes.h"

W65C02S::W65C02S()
{
	sp.b.h = 0x01;
}

bool W65C02S::setProperty(string name, string value)
{
	if (name == "pc")
		pc.w.l = getInt(value);
	else if (name == "sp")
		sp.b.l = getInt(value);
	else if (name == "p")
		p = getInt(value);
	else if (name == "a")
		a = getInt(value);
	else if (name == "x")
		x = getInt(value);
	else if (name == "y")
		y = getInt(value);
	else if (name == "pendingIRQ")
		pendingIRQ = getInt(value);
	else if (name == "afterCLI")
		afterCLI = getInt(value);
	else if (name == "irqCount")
		irqCount = getInt(value);
	else
		return false;
	
	return true;
}

bool W65C02S::getProperty(string name, string &value)
{
	if (name == "pc")
		value = getHex(pc.w.l);
	else if (name == "sp")
		value = getHex(sp.b.l);
	else if (name == "p")
		value = getHex(p);
	else if (name == "a")
		value = getHex(a);
	else if (name == "x")
		value = getHex(x);
	else if (name == "y")
		value = getHex(y);
	else if (name == "pendingIRQ")
		value = pendingIRQ;
	else if (name == "afterCLI")
		value = afterCLI;
	else if (name == "irqCount")
		value = irqCount;
	else
		return false;
	
	return true;
}

bool W65C02S::connect(string name, OEComponent *component)
{
	if (name == "memoryMap")
		memory = component;
	else if (name == "hostSystem")
		// component->addObserver(this);
		;
	else
		return false;
	
	return true;
}

void W65C02S::execute()
{
	int opcode;
	
	switch (opcode)
	{
			W65C02S_OP(00);
			W65C02S_OP(20);
			W65C02S_OP(40);
			W65C02S_OP(60);
			W65C02S_OP(80);
			W65C02S_OP(a0);
			W65C02S_OP(c0);
			W65C02S_OP(e0);
			
			W65C02S_OP(10);
			W65C02S_OP(30);
			W65C02S_OP(50);
			W65C02S_OP(70);
			W65C02S_OP(90);
			W65C02S_OP(b0);
			W65C02S_OP(d0);
			W65C02S_OP(f0);
			
			W65C02S_OP(01);
			W65C02S_OP(21);
			W65C02S_OP(41);
			W65C02S_OP(61);
			W65C02S_OP(81);
			W65C02S_OP(a1);
			W65C02S_OP(c1);
			W65C02S_OP(e1);
			
			W65C02S_OP(11);
			W65C02S_OP(31);
			W65C02S_OP(51);
			W65C02S_OP(71);
			W65C02S_OP(91);
			W65C02S_OP(b1);
			W65C02S_OP(d1);
			W65C02S_OP(f1);
			
			W65C02S_OP(02);
			W65C02S_OP(22);
			W65C02S_OP(42);
			W65C02S_OP(62);
			W65C02S_OP(82);
			W65C02S_OP(a2);
			W65C02S_OP(c2);
			W65C02S_OP(e2);
			
			W65C02S_OP(12);
			W65C02S_OP(32);
			W65C02S_OP(52);
			W65C02S_OP(72);
			W65C02S_OP(92);
			W65C02S_OP(b2);
			W65C02S_OP(d2);
			W65C02S_OP(f2);
			
			W65C02S_OP(03);
			W65C02S_OP(23);
			W65C02S_OP(43);
			W65C02S_OP(63);
			W65C02S_OP(83);
			W65C02S_OP(a3);
			W65C02S_OP(c3);
			W65C02S_OP(e3);
			
			W65C02S_OP(13);
			W65C02S_OP(33);
			W65C02S_OP(53);
			W65C02S_OP(73);
			W65C02S_OP(93);
			W65C02S_OP(b3);
			W65C02S_OP(d3);
			W65C02S_OP(f3);
			
			W65C02S_OP(04);
			W65C02S_OP(24);
			W65C02S_OP(44);
			W65C02S_OP(64);
			W65C02S_OP(84);
			W65C02S_OP(a4);
			W65C02S_OP(c4);
			W65C02S_OP(e4);
			
			W65C02S_OP(14);
			W65C02S_OP(34);
			W65C02S_OP(54);
			W65C02S_OP(74);
			W65C02S_OP(94);
			W65C02S_OP(b4);
			W65C02S_OP(d4);
			W65C02S_OP(f4);
			
			W65C02S_OP(05);
			W65C02S_OP(25);
			W65C02S_OP(45);
			W65C02S_OP(65);
			W65C02S_OP(85);
			W65C02S_OP(a5);
			W65C02S_OP(c5);
			W65C02S_OP(e5);
			
			W65C02S_OP(15);
			W65C02S_OP(35);
			W65C02S_OP(55);
			W65C02S_OP(75);
			W65C02S_OP(95);
			W65C02S_OP(b5);
			W65C02S_OP(d5);
			W65C02S_OP(f5);
			
			W65C02S_OP(06);
			W65C02S_OP(26);
			W65C02S_OP(46);
			W65C02S_OP(66);
			W65C02S_OP(86);
			W65C02S_OP(a6);
			W65C02S_OP(c6);
			W65C02S_OP(e6);
			
			W65C02S_OP(16);
			W65C02S_OP(36);
			W65C02S_OP(56);
			W65C02S_OP(76);
			W65C02S_OP(96);
			W65C02S_OP(b6);
			W65C02S_OP(d6);
			W65C02S_OP(f6);
			
			W65C02S_OP(07);
			W65C02S_OP(27);
			W65C02S_OP(47);
			W65C02S_OP(67);
			W65C02S_OP(87);
			W65C02S_OP(a7);
			W65C02S_OP(c7);
			W65C02S_OP(e7);
			
			W65C02S_OP(17);
			W65C02S_OP(37);
			W65C02S_OP(57);
			W65C02S_OP(77);
			W65C02S_OP(97);
			W65C02S_OP(b7);
			W65C02S_OP(d7);
			W65C02S_OP(f7);
			
			W65C02S_OP(08);
			W65C02S_OP(28);
			W65C02S_OP(48);
			W65C02S_OP(68);
			W65C02S_OP(88);
			W65C02S_OP(a8);
			W65C02S_OP(c8);
			W65C02S_OP(e8);
			
			W65C02S_OP(18);
			W65C02S_OP(38);
			W65C02S_OP(58);
			W65C02S_OP(78);
			W65C02S_OP(98);
			W65C02S_OP(b8);
			W65C02S_OP(d8);
			W65C02S_OP(f8);
			
			W65C02S_OP(09);
			W65C02S_OP(29);
			W65C02S_OP(49);
			W65C02S_OP(69);
			W65C02S_OP(89);
			W65C02S_OP(a9);
			W65C02S_OP(c9);
			W65C02S_OP(e9);
			
			W65C02S_OP(19);
			W65C02S_OP(39);
			W65C02S_OP(59);
			W65C02S_OP(79);
			W65C02S_OP(99);
			W65C02S_OP(b9);
			W65C02S_OP(d9);
			W65C02S_OP(f9);
			
			W65C02S_OP(0a);
			W65C02S_OP(2a);
			W65C02S_OP(4a);
			W65C02S_OP(6a);
			W65C02S_OP(8a);
			W65C02S_OP(aa);
			W65C02S_OP(ca);
			W65C02S_OP(ea);
			
			W65C02S_OP(1a);
			W65C02S_OP(3a);
			W65C02S_OP(5a);
			W65C02S_OP(7a);
			W65C02S_OP(9a);
			W65C02S_OP(ba);
			W65C02S_OP(da);
			W65C02S_OP(fa);
			
			W65C02S_OP(0b);
			W65C02S_OP(2b);
			W65C02S_OP(4b);
			W65C02S_OP(6b);
			W65C02S_OP(8b);
			W65C02S_OP(ab);
			W65C02S_OP(cb);
			W65C02S_OP(eb);
			
			W65C02S_OP(1b);
			W65C02S_OP(3b);
			W65C02S_OP(5b);
			W65C02S_OP(7b);
			W65C02S_OP(9b);
			W65C02S_OP(bb);
			W65C02S_OP(db);
			W65C02S_OP(fb);
			
			W65C02S_OP(0c);
			W65C02S_OP(2c);
			W65C02S_OP(4c);
			W65C02S_OP(6c);
			W65C02S_OP(8c);
			W65C02S_OP(ac);
			W65C02S_OP(cc);
			W65C02S_OP(ec);
			
			W65C02S_OP(1c);
			W65C02S_OP(3c);
			W65C02S_OP(5c);
			W65C02S_OP(7c);
			W65C02S_OP(9c);
			W65C02S_OP(bc);
			W65C02S_OP(dc);
			W65C02S_OP(fc);
			
			W65C02S_OP(0d);
			W65C02S_OP(2d);
			W65C02S_OP(4d);
			W65C02S_OP(6d);
			W65C02S_OP(8d);
			W65C02S_OP(ad);
			W65C02S_OP(cd);
			W65C02S_OP(ed);
			
			W65C02S_OP(1d);
			W65C02S_OP(3d);
			W65C02S_OP(5d);
			W65C02S_OP(7d);
			W65C02S_OP(9d);
			W65C02S_OP(bd);
			W65C02S_OP(dd);
			W65C02S_OP(fd);
			
			W65C02S_OP(0e);
			W65C02S_OP(2e);
			W65C02S_OP(4e);
			W65C02S_OP(6e);
			W65C02S_OP(8e);
			W65C02S_OP(ae);
			W65C02S_OP(ce);
			W65C02S_OP(ee);
			
			W65C02S_OP(1e);
			W65C02S_OP(3e);
			W65C02S_OP(5e);
			W65C02S_OP(7e);
			W65C02S_OP(9e);
			W65C02S_OP(be);
			W65C02S_OP(de);
			W65C02S_OP(fe);
			
			W65C02S_OP(0f);
			W65C02S_OP(2f);
			W65C02S_OP(4f);
			W65C02S_OP(6f);
			W65C02S_OP(8f);
			W65C02S_OP(af);
			W65C02S_OP(cf);
			W65C02S_OP(ef);
			
			W65C02S_OP(1f);
			W65C02S_OP(3f);
			W65C02S_OP(5f);
			W65C02S_OP(7f);
			W65C02S_OP(9f);
			W65C02S_OP(bf);
			W65C02S_OP(df);
			W65C02S_OP(ff);
	}
}
