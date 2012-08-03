
/**
 * libemulation
 * Apple III MOS6502
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates an Apple III MOS6502 microprocessor
 */

#include "AppleIIIMOS6502.h"
#include "AppleIIIMOS6502Opcodes.h"

#include "CPUInterface.h"
#include "MemoryInterface.h"
#include "AppleIIIInterface.h"

AppleIIIMOS6502::AppleIIIMOS6502() : MOS6502()
{
    extendedMemoryBus = NULL;
    extendedMemoryEnabled = false;
    extendedMemoryBank = -1;
}

bool AppleIIIMOS6502::setRef(string name, OEComponent *ref)
{
    if (name == "extendedMemoryBus")
        extendedMemoryBus = ref;
    else
        return MOS6502::setRef(name, ref);
    
    return true;
}

bool AppleIIIMOS6502::init()
{
    if (!extendedMemoryBus)
    {
        logMessage("extendedMemoryBus not connected");
        
        return false;
    }
    
    return MOS6502::init();
}

bool AppleIIIMOS6502::postMessage(OEComponent *sender, int message, void *data)
{
    if (message == APPLEIII_SET_EXTENDEDMEMORYENABLE)
        extendedMemoryEnabled = *((bool *)data);
    else
        return MOS6502::postMessage(sender, message, data);
    
    return true;
}

void AppleIIIMOS6502::execute()
{
    if (powerState != CONTROLBUS_POWERSTATE_ON)
        icount = 0;
    
    if (isReset)
        icount = 0;
    
    OEUnion zp;
    OEUnion ea;
    
    zp.q = 0;
    ea.q = 0;
    
    while (icount > 0)
    {
        bool wasIRQEnabled = isIRQEnabled;
        isIRQEnabled = !(P & F_I);
        
        if (isSpecialCondition)
        {
            if (isIRQ && wasIRQEnabled)
            {
                isIRQEnabled = false;
                
                icount -= 2;
                PUSH(PCH);
                PUSH(PCL);
                PUSH(P & ~F_B);
                P |= F_I;
                PCL = RDMEM(MOS6502_IRQ_VECTOR);
                PCH = RDMEM(MOS6502_IRQ_VECTOR + 1);
                
                updateSpecialCondition();
            }
            else if (isResetTransition)
            {
                isResetTransition = false;
                
                sp.b.l = 0;
                
                icount -= 2;
                PUSH_DISCARD(PCH);
                PUSH_DISCARD(PCL);
                PUSH_DISCARD(P & ~F_B);
                P |= F_I;
                PCL = RDMEM(MOS6502_RST_VECTOR);
                PCH = RDMEM(MOS6502_RST_VECTOR + 1);
                
                updateSpecialCondition();
            }
            else
            {
                isNMITransition = false;
                
                icount -= 2;
                PUSH(PCH);
                PUSH(PCL);
                PUSH(P & ~F_B);
                P |= F_I;
                PCL = RDMEM(MOS6502_NMI_VECTOR);
                PCH = RDMEM(MOS6502_NMI_VECTOR + 1);
                
                updateSpecialCondition();
            }
        }
        else
        {
/*            static bool cap = true;
            
            if ((pc.q & 0xffff) == 0x0801)
                cap = true;
            
            if (cap)
            {
                static FILE *fp = NULL;
                
                if (!fp)
                    fp = fopen("/Users/mressl/cap.txt", "at");
                if (fp)
                {
                    fprintf(fp, "pc=%04x a=%02x x=%02x y=%02x s=%02x p=%02x [%02x %02x %02x]\n",
                            pc.w.l, a, x, y, sp.b.l, p,
                            memoryBus->read(pc.q),
                            memoryBus->read(pc.q + 1),
                            memoryBus->read(pc.q + 2));
                }
            }*/
            
            OEChar opcode = RDOP();
            
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
                    
                    APPLEIIIMOS6502_OP(11);
                    APPLEIIIMOS6502_OP(31);
                    APPLEIIIMOS6502_OP(51);
                    APPLEIIIMOS6502_OP(71);
                    APPLEIIIMOS6502_OP(91);
                    APPLEIIIMOS6502_OP(b1);
                    APPLEIIIMOS6502_OP(d1);
                    APPLEIIIMOS6502_OP(f1);
                    
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

inline void AppleIIIMOS6502::setExtendedMemoryBank(OEInt value)
{
    if (value == extendedMemoryBank)
        return;
    
    AddressOffsetMap offsetMap;
    
    if (value == 0xf)
    {
        offsetMap.startAddress = 0x0000;
        offsetMap.endAddress = 0x1fff;
        offsetMap.offset = APPLEIII_SYSTEMBANK * 0x8000;

        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0x2000;
        offsetMap.endAddress = 0x9fff;
        offsetMap.offset = -0x2000;
        
        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0xa000;
        offsetMap.endAddress = 0xffff;
        offsetMap.offset = APPLEIII_SYSTEMBANK * 0x8000 - 0x8000;
        
        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    }
    else
    {
        if (value == APPLEIII_SYSTEMBANK)
            value = 0xf;
        
        offsetMap.startAddress = 0x0000;
        offsetMap.endAddress = 0x7fff;
        offsetMap.offset = value * 0x8000;
        
        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0x8000;
        offsetMap.endAddress = 0xffff;
        offsetMap.offset = (value + 1) * 0x8000 - 0x8000;
        
        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    }
    
    extendedMemoryBank = value;
}
