
/**
 * libemulation
 * MOS6502
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MOS6502 microprocessor.
 */

#include "CPUInterface.h"
#include "ControlBus.h"

#include "MOS6502.h"
#include "MOS6502Opcodes.h"

MOS6502::MOS6502()
{
	controlBus = NULL;
	memoryBus = NULL;
	
	a = 0;
	x = 0;
	y = 0;
	p = 0;
	pc.q = 0;
	sp.q = 0x1ff;
    
    isReset = false;
    isIRQ = false;
    isNMI = false;
    
    icount = 0;
}

bool MOS6502::setValue(string name, string value)
{
	if (name == "a")
		a = getUInt(value);
	else if (name == "x")
		x = getUInt(value);
	else if (name == "y")
		y = getUInt(value);
	else if (name == "s")
		sp.b.l = getUInt(value);
	else if (name == "p")
		p = getUInt(value);
	else if (name == "pc")
		pc.w.l = getUInt(value);
	else
		return false;
	
	return true;
}

bool MOS6502::getValue(string name, string& value)
{
	if (name == "a")
		value = getHexString(a);
	else if (name == "x")
		value = getHexString(x);
	else if (name == "y")
		value = getHexString(y);
	else if (name == "s")
		value = getHexString(sp.b.l);
	else if (name == "p")
		value = getHexString(p);
	else if (name == "pc")
		value = getHexString(pc.w.l);
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
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_CLEAR);
            controlBus->removeObserver(this, CONTROLBUS_IRQ_DID_ASSERT);
            controlBus->removeObserver(this, CONTROLBUS_IRQ_DID_CLEAR);
            controlBus->removeObserver(this, CONTROLBUS_NMI_DID_ASSERT);
        }
		controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_CLEAR);
            controlBus->addObserver(this, CONTROLBUS_IRQ_DID_ASSERT);
            controlBus->addObserver(this, CONTROLBUS_IRQ_DID_CLEAR);
            controlBus->addObserver(this, CONTROLBUS_NMI_DID_ASSERT);
        }
	}
	else
		return false;
	
	return true;
}

bool MOS6502::init()
{
    if (controlBus)
    {
        controlBus->postMessage(this, CONTROLBUS_IS_RESET_ASSERTED, &isReset);
        controlBus->postMessage(this, CONTROLBUS_IS_IRQ_ASSERTED, &isIRQ);
    }
    
    return true;
}

bool MOS6502::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case CPU_RUN_CYCLES:
            icount = *((OEInt64 *)data);
            execute();
            *((OEInt64 *)data) = icount;
            return true;
            
        case CPU_SET_CYCLES:
            icount = *((OEInt64 *)data);
            return true;
            
        case CPU_GET_CYCLES:
            *((OEInt64 *)data) = icount;
            return true;
    }
    
    return false;
}

void MOS6502::notify(OEComponent *sender, int notification, void *data)
{
	switch (notification)
	{
		case CONTROLBUS_RESET_DID_ASSERT:
            isReset = true;
			return;
            
        case CONTROLBUS_RESET_DID_CLEAR:
            isReset = false;
            
            p = F_T | F_I | F_Z | F_B | (P & F_D);
            PCL = RDMEM(MOS6502_RST_VECTOR);
            PCH = RDMEM(MOS6502_RST_VECTOR + 1);
            sp.q = 0x1ff;
            
            return;
			
		case CONTROLBUS_IRQ_DID_ASSERT:
            isIRQ = true;
 			return;
			
		case CONTROLBUS_IRQ_DID_CLEAR:
            isIRQ = false;
			return;
			
		case CONTROLBUS_NMI_DID_ASSERT:
            isNMI = true;
			return;
	}
}

void MOS6502::execute()
{
	while (icount > 0)
	{
        bool wasIRQEnabled = isIRQEnabled;
        isIRQEnabled = !(P & F_I);
        
        if (isReset)
            icount = 0;
        else if (isNMI)
        {
            isNMI = false;
            
            icount -= 2;
            PUSH(PCH);
            PUSH(PCL);
            PUSH(P & ~F_B);
            // clear D flag, set I flag
            P = (P & ~F_D) | F_I;
            PCL = RDMEM(MOS6502_NMI_VECTOR);
            PCH = RDMEM(MOS6502_NMI_VECTOR + 1);
        }
        else if (isIRQ && wasIRQEnabled)
        {
			icount -= 2;
			PUSH(PCH);
			PUSH(PCL);
			PUSH(P & ~F_B);
            // set I flag
			P |= F_I;
            PCL = RDMEM(MOS6502_IRQ_VECTOR);
			PCH = RDMEM(MOS6502_IRQ_VECTOR + 1);
        }
        else
        {
            OEUnion zp;
            OEUnion ea;
            
            OEComponent::notify(this, CPU_INSTRUCTION_WILL_EXECUTE, &pc);
            
            if (pc.q != 0)
            {
                int a = 4;
                a = 5;
            }
            
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
        }
	};
}
