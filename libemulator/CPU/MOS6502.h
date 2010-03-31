
/**
 * libemulator
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a MOS6502 microprocessor.
 */

#include "OEComponent.h"

class MOS6502 : public OEComponent
{
public:
	MOS6502();
	
	int ioctl(int message, void *data);		
	
private:
	OEComponent *memory;
	
	OEPair ppc;
	OEPair pc;
	OEPair sp;
	OEPair zp;
	OEPair ea;
	
	UINT8 a;
	UINT8 x;
	UINT8 y;
	UINT8 p;

	int irqCount;

	UINT8 pending_irq;
	UINT8 after_cli;
	UINT8 nmi_state;
	UINT8 so_state;
	
	int icount;
	
#undef OP
#define OP(nn) void MOS602::m6502_##nn()
	
	static void (MOS6502::*const p6502[0x100])() =
	{
		OP(00), OP(01), OP(02), OP(03), OP(04), OP(05), OP(06), OP(07),
		OP(08), OP(09), OP(0a), OP(0b), OP(0c), OP(0d), OP(0e), OP(0f),
		OP(10), OP(11), OP(12), OP(13), OP(14), OP(15), OP(16), OP(17),
		OP(18), OP(19), OP(1a), OP(1b), OP(1c), OP(1d), OP(1e), OP(1f),
		OP(20), OP(21), OP(22), OP(23), OP(24), OP(25), OP(26), OP(27),
		OP(28), OP(29), OP(2a), OP(2b), OP(2c), OP(2d), OP(2e), OP(2f),
		OP(30), OP(31), OP(32), OP(33), OP(34), OP(35), OP(36), OP(37),
		OP(38), OP(39), OP(3a), OP(3b), OP(3c), OP(3d), OP(3e), OP(3f),
		OP(40), OP(41), OP(42), OP(43), OP(44), OP(45), OP(46), OP(47),
		OP(48), OP(49), OP(4a), OP(4b), OP(4c), OP(4d), OP(4e), OP(4f),
		OP(50), OP(51), OP(52), OP(53), OP(54), OP(55), OP(56), OP(57),
		OP(58), OP(59), OP(5a), OP(5b), OP(5c), OP(5d), OP(5e), OP(5f),
		OP(60), OP(61), OP(62), OP(63), OP(64), OP(65), OP(66), OP(67),
		OP(68), OP(69), OP(6a), OP(6b), OP(6c), OP(6d), OP(6e), OP(6f),
		OP(70), OP(71), OP(72), OP(73), OP(74), OP(75), OP(76), OP(77),
		OP(78), OP(79), OP(7a), OP(7b), OP(7c), OP(7d), OP(7e), OP(7f),
		
		OP(80), OP(81), OP(82), OP(83), OP(84), OP(85), OP(86), OP(87),
		OP(88), OP(89), OP(8a), OP(8b), OP(8c), OP(8d), OP(8e), OP(8f),
		OP(90), OP(91), OP(92), OP(93), OP(94), OP(95), OP(96), OP(97),
		OP(98), OP(99), OP(9a), OP(9b), OP(9c), OP(9d), OP(9e), OP(9f),
		OP(a0), OP(a1), OP(a2), OP(a3), OP(a4), OP(a5), OP(a6), OP(a7),
		OP(a8), OP(a9), OP(aa), OP(ab), OP(ac), OP(ad), OP(ae), OP(af),
		OP(b0), OP(b1), OP(b2), OP(b3), OP(b4), OP(b5), OP(b6), OP(b7),
		OP(b8), OP(b9), OP(ba), OP(bb), OP(bc), OP(bd), OP(be), OP(bf),
		OP(c0), OP(c1), OP(c2), OP(c3), OP(c4), OP(c5), OP(c6), OP(c7),
		OP(c8), OP(c9), OP(ca), OP(cb), OP(cc), OP(cd), OP(ce), OP(cf),
		OP(d0), OP(d1), OP(d2), OP(d3), OP(d4), OP(d5), OP(d6), OP(d7),
		OP(d8), OP(d9), OP(da), OP(db), OP(dc), OP(dd), OP(de), OP(df),
		OP(e0), OP(e1), OP(e2), OP(e3), OP(e4), OP(e5), OP(e6), OP(e7),
		OP(e8), OP(e9), OP(ea), OP(eb), OP(ec), OP(ed), OP(ee), OP(ef),
		OP(f0), OP(f1), OP(f2), OP(f3), OP(f4), OP(f5), OP(f6), OP(f7),
		OP(f8), OP(f9), OP(fa), OP(fb), OP(fc), OP(fd), OP(fe), OP(ff),
	};
	
#undef OP
#define OP(nn) void m6502_##nn()
	
	OP(00); OP(01); OP(02); OP(03); OP(04); OP(05); OP(06); OP(07);
	OP(08); OP(09); OP(0a); OP(0b); OP(0c); OP(0d); OP(0e); OP(0f);
	OP(10); OP(11); OP(12); OP(13); OP(14); OP(15); OP(16); OP(17);
	OP(18); OP(19); OP(1a); OP(1b); OP(1c); OP(1d); OP(1e); OP(1f);
	OP(20); OP(21); OP(22); OP(23); OP(24); OP(25); OP(26); OP(27);
	OP(28); OP(29); OP(2a); OP(2b); OP(2c); OP(2d); OP(2e); OP(2f);
	OP(30); OP(31); OP(32); OP(33); OP(34); OP(35); OP(36); OP(37);
	OP(38); OP(39); OP(3a); OP(3b); OP(3c); OP(3d); OP(3e); OP(3f);
	OP(40); OP(41); OP(42); OP(43); OP(44); OP(45); OP(46); OP(47);
	OP(48); OP(49); OP(4a); OP(4b); OP(4c); OP(4d); OP(4e); OP(4f);
	OP(50); OP(51); OP(52); OP(53); OP(54); OP(55); OP(56); OP(57);
	OP(58); OP(59); OP(5a); OP(5b); OP(5c); OP(5d); OP(5e); OP(5f);
	OP(60); OP(61); OP(62); OP(63); OP(64); OP(65); OP(66); OP(67);
	OP(68); OP(69); OP(6a); OP(6b); OP(6c); OP(6d); OP(6e); OP(6f);
	OP(70); OP(71); OP(72); OP(73); OP(74); OP(75); OP(76); OP(77);
	OP(78); OP(79); OP(7a); OP(7b); OP(7c); OP(7d); OP(7e); OP(7f);
	
	OP(80); OP(81); OP(82); OP(83); OP(84); OP(85); OP(86); OP(87);
	OP(88); OP(89); OP(8a); OP(8b); OP(8c); OP(8d); OP(8e); OP(8f);
	OP(90); OP(91); OP(92); OP(93); OP(94); OP(95); OP(96); OP(97);
	OP(98); OP(99); OP(9a); OP(9b); OP(9c); OP(9d); OP(9e); OP(9f);
	OP(a0); OP(a1); OP(a2); OP(a3); OP(a4); OP(a5); OP(a6); OP(a7);
	OP(a8); OP(a9); OP(aa); OP(ab); OP(ac); OP(ad); OP(ae); OP(af);
	OP(b0); OP(b1); OP(b2); OP(b3); OP(b4); OP(b5); OP(b6); OP(b7);
	OP(b8); OP(b9); OP(ba); OP(bb); OP(bc); OP(bd); OP(be); OP(bf);
	OP(c0); OP(c1); OP(c2); OP(c3); OP(c4); OP(c5); OP(c6); OP(c7);
	OP(c8); OP(c9); OP(ca); OP(cb); OP(cc); OP(cd); OP(ce); OP(cf);
	OP(d0); OP(d1); OP(d2); OP(d3); OP(d4); OP(d5); OP(d6); OP(d7);
	OP(d8); OP(d9); OP(da); OP(db); OP(dc); OP(dd); OP(de); OP(df);
	OP(e0); OP(e1); OP(e2); OP(e3); OP(e4); OP(e5); OP(e6); OP(e7);
	OP(e8); OP(e9); OP(ea); OP(eb); OP(ec); OP(ed); OP(ee); OP(ef);
	OP(f0); OP(f1); OP(f2); OP(f3); OP(f4); OP(f5); OP(f6); OP(f7);
	OP(f8); OP(f9); OP(fa); OP(fb); OP(fc); OP(fd); OP(fe); OP(ff);
};
