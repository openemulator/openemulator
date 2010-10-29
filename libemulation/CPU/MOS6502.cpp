
/**
 * libemulation
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor.
 */

#include "CPUInterface.h"
#include "ControlBusInterface.h"

#include "MOS6502.h"
#include "MOS6502Opcodes.h"

MOS6502::MOS6502()
{
	a = 0;
	x = 0;
	y = 0;
	p = 0;
	pc.d = 0;
	sp.d = 0x100;
	zp.d = 0;
	ea.d = 0;
	
	memoryBus = NULL;
	controlBus = NULL;
}

bool MOS6502::setValue(string name, string value)
{
	if (name == "a")
		a = getInt(value);
	else if (name == "x")
		x = getInt(value);
	else if (name == "y")
		y = getInt(value);
	else if (name == "s")
		sp.b.l = getInt(value);
	else if (name == "p")
		p = getInt(value);
	else if (name == "pc")
		pc.w.l = getInt(value);
	else
		return false;
	
	return true;
}

bool MOS6502::getValue(string name, string &value)
{
	if (name == "a")
		value = getHex(a);
	else if (name == "x")
		value = getHex(x);
	else if (name == "y")
		value = getHex(y);
	else if (name == "s")
		value = getHex(sp.b.l);
	else if (name == "p")
		value = getHex(p);
	else if (name == "pc")
		value = getHex(pc.w.l);
	else
		return false;
	
	return true;
}

bool MOS6502::setRef(string name, OEComponent *ref)
{
	if (name == "memoryBus")
		memoryBus = ref;
	else if (name == "controlBus")
	{
		if (controlBus)
		{
			controlBus->removeObserver(this, CONTROLBUS_RESET_CHANGED);
			controlBus->removeObserver(this, CONTROLBUS_IRQ_CHANGED);
			controlBus->removeObserver(this, CONTROLBUS_NMI_CHANGED);
		}
		controlBus = ref;
		if (controlBus)
		{
			controlBus->addObserver(this, CONTROLBUS_RESET_CHANGED);
			controlBus->addObserver(this, CONTROLBUS_IRQ_CHANGED);
			controlBus->addObserver(this, CONTROLBUS_NMI_CHANGED);
		}
	}
	else
		return false;
	
	return true;
}

void MOS6502::notify(OEComponent *sender, int notification, void *data)
{
	switch (notification)
	{
		case CONTROLBUS_RESET_CHANGED:
			if (*((bool *) data))
				reset();
			break;
			
		case CONTROLBUS_IRQ_CHANGED:
			break;
			
		case CONTROLBUS_NMI_CHANGED:
			break;
	}
}

void MOS6502::reset()
{
	sp.b.l = 0xff;
	p = F_T|F_I|F_Z|F_B|(P&F_D);
	pendingIRQ = 0;
	afterCLI = 0;
	irqCount = 0;
}

void MOS6502::assertIRQ()
{
/*	if(irqline == M6502_SET_OVERFLOW)
	{
		if( cpustate->so_state && !state )
		{
			LOG(( "M6502 '%s' set overflow\n", cpustate->device->tag));
			P|=F_V;
		}
		cpustate->so_state=state;
		return;
	}
	cpustate->irq_state = state;
	if( state != CLEAR_LINE )
	{
		LOG(( "M6502 '%s' set_irq_line(ASSERT)\n", cpustate->device->tag));
		cpustate->pending_irq = 1;
		//          cpustate->pending_irq = 2;
		//cpustate->int_occured = icount;
	}*/
}

void MOS6502::assertNMI()
{
	//	OELog("M6502 set_nmi_line(ASSERT)\n");
	EAD = MOS6502_NMI_VEC;
	icount -= 2;
	PUSH(PCH);
	PUSH(PCL);
	PUSH(P & ~F_B);
	P |= F_I;		/* set I flag */
	PCL = RDMEM(EAD);
	PCH = RDMEM(EAD+1);
	//	OELog("M6502 takes NMI (" + PCD + ")\n");
}

void MOS6502::execute()
{
	icount = 1000;
	
	// Take IRQ
	if( !(P & F_I) )
	{
	
	}
	
	do
	{
//		if (debuggerHook)
//			debuggerHook();
		
		OEUInt8 opcode = RDOP();
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
			MOS6502_OP(61);
			MOS6502_OP(81);
			MOS6502_OP(a1);
			MOS6502_OP(c1);
			MOS6502_OP(e1);
				
			MOS6502_OP(11);
			MOS6502_OP(31);
			MOS6502_OP(51);
			MOS6502_OP(71);
			MOS6502_OP(91);
			MOS6502_OP(b1);
			MOS6502_OP(d1);
			MOS6502_OP(f1);
				
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
			MOS6502_OP(63);
			MOS6502_OP(83);
			MOS6502_OP(a3);
			MOS6502_OP(c3);
			MOS6502_OP(e3);
				
			MOS6502_OP(13);
			MOS6502_OP(33);
			MOS6502_OP(53);
			MOS6502_OP(73);
			MOS6502_OP(93);
			MOS6502_OP(b3);
			MOS6502_OP(d3);
			MOS6502_OP(f3);
			
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
			MOS6502_OP(65);
			MOS6502_OP(85);
			MOS6502_OP(a5);
			MOS6502_OP(c5);
			MOS6502_OP(e5);
				
			MOS6502_OP(15);
			MOS6502_OP(35);
			MOS6502_OP(55);
			MOS6502_OP(75);
			MOS6502_OP(95);
			MOS6502_OP(b5);
			MOS6502_OP(d5);
			MOS6502_OP(f5);
				
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
			MOS6502_OP(67);
			MOS6502_OP(87);
			MOS6502_OP(a7);
			MOS6502_OP(c7);
			MOS6502_OP(e7);
				
			MOS6502_OP(17);
			MOS6502_OP(37);
			MOS6502_OP(57);
			MOS6502_OP(77);
			MOS6502_OP(97);
			MOS6502_OP(b7);
			MOS6502_OP(d7);
			MOS6502_OP(f7);
				
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
			MOS6502_OP(69);
			MOS6502_OP(89);
			MOS6502_OP(a9);
			MOS6502_OP(c9);
			MOS6502_OP(e9);
				
			MOS6502_OP(19);
			MOS6502_OP(39);
			MOS6502_OP(59);
			MOS6502_OP(79);
			MOS6502_OP(99);
			MOS6502_OP(b9);
			MOS6502_OP(d9);
			MOS6502_OP(f9);
				
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
			MOS6502_OP(6b);
			MOS6502_OP(8b);
			MOS6502_OP(ab);
			MOS6502_OP(cb);
			MOS6502_OP(eb);
				
			MOS6502_OP(1b);
			MOS6502_OP(3b);
			MOS6502_OP(5b);
			MOS6502_OP(7b);
			MOS6502_OP(9b);
			MOS6502_OP(bb);
			MOS6502_OP(db);
			MOS6502_OP(fb);
				
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
			MOS6502_OP(9c);
			MOS6502_OP(bc);
			MOS6502_OP(dc);
			MOS6502_OP(fc);
				
			MOS6502_OP(0d);
			MOS6502_OP(2d);
			MOS6502_OP(4d);
			MOS6502_OP(6d);
			MOS6502_OP(8d);
			MOS6502_OP(ad);
			MOS6502_OP(cd);
			MOS6502_OP(ed);
				
			MOS6502_OP(1d);
			MOS6502_OP(3d);
			MOS6502_OP(5d);
			MOS6502_OP(7d);
			MOS6502_OP(9d);
			MOS6502_OP(bd);
			MOS6502_OP(dd);
			MOS6502_OP(fd);
				
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
			MOS6502_OP(9e);
			MOS6502_OP(be);
			MOS6502_OP(de);
			MOS6502_OP(fe);
				
			MOS6502_OP(0f);
			MOS6502_OP(2f);
			MOS6502_OP(4f);
			MOS6502_OP(6f);
			MOS6502_OP(8f);
			MOS6502_OP(af);
			MOS6502_OP(cf);
			MOS6502_OP(ef);
				
			MOS6502_OP(1f);
			MOS6502_OP(3f);
			MOS6502_OP(5f);
			MOS6502_OP(7f);
			MOS6502_OP(9f);
			MOS6502_OP(bf);
			MOS6502_OP(df);
			MOS6502_OP(ff);
		}
	} while (icount > 0);
}
