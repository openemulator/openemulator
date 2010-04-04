
/**
 * libemulator
 * Z80Operations
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Z80 operations.
 */

#define CF		0x01
#define NF		0x02
#define PF		0x04
#define VF		PF
#define XF		0x08
#define HF		0x10
#define YF		0x20
#define ZF		0x40
#define SF		0x80

#define INT_IRQ 0x01
#define NMI_IRQ 0x02

#define PRVPC	ppc.d		/* previous program counter */

#define PCD		pc.d
#define PC		pc.w.l

#define SPD 	sp.d
#define SP		sp.w.l

#define AFD 	af.d
#define AF		af.w.l
#define A		af.b.h
#define F		af.b.l

#define BCD 	bc.d
#define BC		bc.w.l
#define B		bc.b.h
#define C		bc.b.l

#define DED 	de.d
#define DE		de.w.l
#define D		de.b.h
#define E		de.b.l

#define HLD 	hl.d
#define HL		hl.w.l
#define H		hl.b.h
#define L		hl.b.l

#define IXD 	ix.d
#define IX		ix.w.l
#define HX		ix.b.h
#define LX		ix.b.l

#define IYD 	iy.d
#define IY		iy.w.l
#define HY		iy.b.h
#define LY		iy.b.l

#define WZ		wz.w.l
#define WZ_H	wz.b.h
#define WZ_L	wz.b.l

static UINT8 SZ[256];		/* zero and sign flags */
static UINT8 SZ_BIT[256];	/* zero, sign and parity/overflow (=zero) flags for BIT opcode */
static UINT8 SZP[256];		/* zero, sign and parity flags */
static UINT8 SZHV_inc[256]; /* zero, sign, half carry and overflow flags INC r8 */
static UINT8 SZHV_dec[256]; /* zero, sign, half carry and overflow flags DEC r8 */

static UINT8 *SZHVC_add = 0;
static UINT8 *SZHVC_sub = 0;

static const UINT8 cc_op[0x100] = {
4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11};

static const UINT8 cc_cb[0x100] = {
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8};

static const UINT8 cc_ed[0x100] = {
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9,
12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9,
12,12,15,20, 8,14, 8,18,12,12,15,20, 8,14, 8,18,
12,12,15,20, 8,14, 8, 8,12,12,15,20, 8,14, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

static const UINT8 cc_xy[0x100] = {
4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
4,14,20,10, 9, 9,11, 4, 4,15,20,10, 9, 9,11, 4,
4, 4, 4, 4,23,23,19, 4, 4,15, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
19,19,19,19,19,19, 4,19, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
4,14, 4,23, 4,15, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4, 4,10, 4, 4, 4, 4, 4, 4};

static const UINT8 cc_xycb[0x100] = {
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23};

/* extra cycles if jr/jp/call taken and 'interrupt latency' on rst 0-7 */
static const UINT8 cc_ex[0x100] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* DJNZ */
5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,	/* JR NZ/JR Z */
5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,	/* JR NC/JR C */
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0,	/* LDIR/CPIR/INIR/OTIR LDDR/CPDR/INDR/OTDR */
6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2};

#define cc_dd	cc_xy
#define cc_fd	cc_xy

static void take_interrupt(z80_state *z80);
static CPU_BURN( z80 );

typedef void (*funcptr)(z80_state *z80);

#define PROTOTYPES(tablename,prefix) \
INLINE void prefix##_00(z80_state *z80); INLINE void prefix##_01(z80_state *z80); INLINE void prefix##_02(z80_state *z80); INLINE void prefix##_03(z80_state *z80); \
INLINE void prefix##_04(z80_state *z80); INLINE void prefix##_05(z80_state *z80); INLINE void prefix##_06(z80_state *z80); INLINE void prefix##_07(z80_state *z80); \
INLINE void prefix##_08(z80_state *z80); INLINE void prefix##_09(z80_state *z80); INLINE void prefix##_0a(z80_state *z80); INLINE void prefix##_0b(z80_state *z80); \
INLINE void prefix##_0c(z80_state *z80); INLINE void prefix##_0d(z80_state *z80); INLINE void prefix##_0e(z80_state *z80); INLINE void prefix##_0f(z80_state *z80); \
INLINE void prefix##_10(z80_state *z80); INLINE void prefix##_11(z80_state *z80); INLINE void prefix##_12(z80_state *z80); INLINE void prefix##_13(z80_state *z80); \
INLINE void prefix##_14(z80_state *z80); INLINE void prefix##_15(z80_state *z80); INLINE void prefix##_16(z80_state *z80); INLINE void prefix##_17(z80_state *z80); \
INLINE void prefix##_18(z80_state *z80); INLINE void prefix##_19(z80_state *z80); INLINE void prefix##_1a(z80_state *z80); INLINE void prefix##_1b(z80_state *z80); \
INLINE void prefix##_1c(z80_state *z80); INLINE void prefix##_1d(z80_state *z80); INLINE void prefix##_1e(z80_state *z80); INLINE void prefix##_1f(z80_state *z80); \
INLINE void prefix##_20(z80_state *z80); INLINE void prefix##_21(z80_state *z80); INLINE void prefix##_22(z80_state *z80); INLINE void prefix##_23(z80_state *z80); \
INLINE void prefix##_24(z80_state *z80); INLINE void prefix##_25(z80_state *z80); INLINE void prefix##_26(z80_state *z80); INLINE void prefix##_27(z80_state *z80); \
INLINE void prefix##_28(z80_state *z80); INLINE void prefix##_29(z80_state *z80); INLINE void prefix##_2a(z80_state *z80); INLINE void prefix##_2b(z80_state *z80); \
INLINE void prefix##_2c(z80_state *z80); INLINE void prefix##_2d(z80_state *z80); INLINE void prefix##_2e(z80_state *z80); INLINE void prefix##_2f(z80_state *z80); \
INLINE void prefix##_30(z80_state *z80); INLINE void prefix##_31(z80_state *z80); INLINE void prefix##_32(z80_state *z80); INLINE void prefix##_33(z80_state *z80); \
INLINE void prefix##_34(z80_state *z80); INLINE void prefix##_35(z80_state *z80); INLINE void prefix##_36(z80_state *z80); INLINE void prefix##_37(z80_state *z80); \
INLINE void prefix##_38(z80_state *z80); INLINE void prefix##_39(z80_state *z80); INLINE void prefix##_3a(z80_state *z80); INLINE void prefix##_3b(z80_state *z80); \
INLINE void prefix##_3c(z80_state *z80); INLINE void prefix##_3d(z80_state *z80); INLINE void prefix##_3e(z80_state *z80); INLINE void prefix##_3f(z80_state *z80); \
INLINE void prefix##_40(z80_state *z80); INLINE void prefix##_41(z80_state *z80); INLINE void prefix##_42(z80_state *z80); INLINE void prefix##_43(z80_state *z80); \
INLINE void prefix##_44(z80_state *z80); INLINE void prefix##_45(z80_state *z80); INLINE void prefix##_46(z80_state *z80); INLINE void prefix##_47(z80_state *z80); \
INLINE void prefix##_48(z80_state *z80); INLINE void prefix##_49(z80_state *z80); INLINE void prefix##_4a(z80_state *z80); INLINE void prefix##_4b(z80_state *z80); \
INLINE void prefix##_4c(z80_state *z80); INLINE void prefix##_4d(z80_state *z80); INLINE void prefix##_4e(z80_state *z80); INLINE void prefix##_4f(z80_state *z80); \
INLINE void prefix##_50(z80_state *z80); INLINE void prefix##_51(z80_state *z80); INLINE void prefix##_52(z80_state *z80); INLINE void prefix##_53(z80_state *z80); \
INLINE void prefix##_54(z80_state *z80); INLINE void prefix##_55(z80_state *z80); INLINE void prefix##_56(z80_state *z80); INLINE void prefix##_57(z80_state *z80); \
INLINE void prefix##_58(z80_state *z80); INLINE void prefix##_59(z80_state *z80); INLINE void prefix##_5a(z80_state *z80); INLINE void prefix##_5b(z80_state *z80); \
INLINE void prefix##_5c(z80_state *z80); INLINE void prefix##_5d(z80_state *z80); INLINE void prefix##_5e(z80_state *z80); INLINE void prefix##_5f(z80_state *z80); \
INLINE void prefix##_60(z80_state *z80); INLINE void prefix##_61(z80_state *z80); INLINE void prefix##_62(z80_state *z80); INLINE void prefix##_63(z80_state *z80); \
INLINE void prefix##_64(z80_state *z80); INLINE void prefix##_65(z80_state *z80); INLINE void prefix##_66(z80_state *z80); INLINE void prefix##_67(z80_state *z80); \
INLINE void prefix##_68(z80_state *z80); INLINE void prefix##_69(z80_state *z80); INLINE void prefix##_6a(z80_state *z80); INLINE void prefix##_6b(z80_state *z80); \
INLINE void prefix##_6c(z80_state *z80); INLINE void prefix##_6d(z80_state *z80); INLINE void prefix##_6e(z80_state *z80); INLINE void prefix##_6f(z80_state *z80); \
INLINE void prefix##_70(z80_state *z80); INLINE void prefix##_71(z80_state *z80); INLINE void prefix##_72(z80_state *z80); INLINE void prefix##_73(z80_state *z80); \
INLINE void prefix##_74(z80_state *z80); INLINE void prefix##_75(z80_state *z80); INLINE void prefix##_76(z80_state *z80); INLINE void prefix##_77(z80_state *z80); \
INLINE void prefix##_78(z80_state *z80); INLINE void prefix##_79(z80_state *z80); INLINE void prefix##_7a(z80_state *z80); INLINE void prefix##_7b(z80_state *z80); \
INLINE void prefix##_7c(z80_state *z80); INLINE void prefix##_7d(z80_state *z80); INLINE void prefix##_7e(z80_state *z80); INLINE void prefix##_7f(z80_state *z80); \
INLINE void prefix##_80(z80_state *z80); INLINE void prefix##_81(z80_state *z80); INLINE void prefix##_82(z80_state *z80); INLINE void prefix##_83(z80_state *z80); \
INLINE void prefix##_84(z80_state *z80); INLINE void prefix##_85(z80_state *z80); INLINE void prefix##_86(z80_state *z80); INLINE void prefix##_87(z80_state *z80); \
INLINE void prefix##_88(z80_state *z80); INLINE void prefix##_89(z80_state *z80); INLINE void prefix##_8a(z80_state *z80); INLINE void prefix##_8b(z80_state *z80); \
INLINE void prefix##_8c(z80_state *z80); INLINE void prefix##_8d(z80_state *z80); INLINE void prefix##_8e(z80_state *z80); INLINE void prefix##_8f(z80_state *z80); \
INLINE void prefix##_90(z80_state *z80); INLINE void prefix##_91(z80_state *z80); INLINE void prefix##_92(z80_state *z80); INLINE void prefix##_93(z80_state *z80); \
INLINE void prefix##_94(z80_state *z80); INLINE void prefix##_95(z80_state *z80); INLINE void prefix##_96(z80_state *z80); INLINE void prefix##_97(z80_state *z80); \
INLINE void prefix##_98(z80_state *z80); INLINE void prefix##_99(z80_state *z80); INLINE void prefix##_9a(z80_state *z80); INLINE void prefix##_9b(z80_state *z80); \
INLINE void prefix##_9c(z80_state *z80); INLINE void prefix##_9d(z80_state *z80); INLINE void prefix##_9e(z80_state *z80); INLINE void prefix##_9f(z80_state *z80); \
INLINE void prefix##_a0(z80_state *z80); INLINE void prefix##_a1(z80_state *z80); INLINE void prefix##_a2(z80_state *z80); INLINE void prefix##_a3(z80_state *z80); \
INLINE void prefix##_a4(z80_state *z80); INLINE void prefix##_a5(z80_state *z80); INLINE void prefix##_a6(z80_state *z80); INLINE void prefix##_a7(z80_state *z80); \
INLINE void prefix##_a8(z80_state *z80); INLINE void prefix##_a9(z80_state *z80); INLINE void prefix##_aa(z80_state *z80); INLINE void prefix##_ab(z80_state *z80); \
INLINE void prefix##_ac(z80_state *z80); INLINE void prefix##_ad(z80_state *z80); INLINE void prefix##_ae(z80_state *z80); INLINE void prefix##_af(z80_state *z80); \
INLINE void prefix##_b0(z80_state *z80); INLINE void prefix##_b1(z80_state *z80); INLINE void prefix##_b2(z80_state *z80); INLINE void prefix##_b3(z80_state *z80); \
INLINE void prefix##_b4(z80_state *z80); INLINE void prefix##_b5(z80_state *z80); INLINE void prefix##_b6(z80_state *z80); INLINE void prefix##_b7(z80_state *z80); \
INLINE void prefix##_b8(z80_state *z80); INLINE void prefix##_b9(z80_state *z80); INLINE void prefix##_ba(z80_state *z80); INLINE void prefix##_bb(z80_state *z80); \
INLINE void prefix##_bc(z80_state *z80); INLINE void prefix##_bd(z80_state *z80); INLINE void prefix##_be(z80_state *z80); INLINE void prefix##_bf(z80_state *z80); \
INLINE void prefix##_c0(z80_state *z80); INLINE void prefix##_c1(z80_state *z80); INLINE void prefix##_c2(z80_state *z80); INLINE void prefix##_c3(z80_state *z80); \
INLINE void prefix##_c4(z80_state *z80); INLINE void prefix##_c5(z80_state *z80); INLINE void prefix##_c6(z80_state *z80); INLINE void prefix##_c7(z80_state *z80); \
INLINE void prefix##_c8(z80_state *z80); INLINE void prefix##_c9(z80_state *z80); INLINE void prefix##_ca(z80_state *z80); INLINE void prefix##_cb(z80_state *z80); \
INLINE void prefix##_cc(z80_state *z80); INLINE void prefix##_cd(z80_state *z80); INLINE void prefix##_ce(z80_state *z80); INLINE void prefix##_cf(z80_state *z80); \
INLINE void prefix##_d0(z80_state *z80); INLINE void prefix##_d1(z80_state *z80); INLINE void prefix##_d2(z80_state *z80); INLINE void prefix##_d3(z80_state *z80); \
INLINE void prefix##_d4(z80_state *z80); INLINE void prefix##_d5(z80_state *z80); INLINE void prefix##_d6(z80_state *z80); INLINE void prefix##_d7(z80_state *z80); \
INLINE void prefix##_d8(z80_state *z80); INLINE void prefix##_d9(z80_state *z80); INLINE void prefix##_da(z80_state *z80); INLINE void prefix##_db(z80_state *z80); \
INLINE void prefix##_dc(z80_state *z80); INLINE void prefix##_dd(z80_state *z80); INLINE void prefix##_de(z80_state *z80); INLINE void prefix##_df(z80_state *z80); \
INLINE void prefix##_e0(z80_state *z80); INLINE void prefix##_e1(z80_state *z80); INLINE void prefix##_e2(z80_state *z80); INLINE void prefix##_e3(z80_state *z80); \
INLINE void prefix##_e4(z80_state *z80); INLINE void prefix##_e5(z80_state *z80); INLINE void prefix##_e6(z80_state *z80); INLINE void prefix##_e7(z80_state *z80); \
INLINE void prefix##_e8(z80_state *z80); INLINE void prefix##_e9(z80_state *z80); INLINE void prefix##_ea(z80_state *z80); INLINE void prefix##_eb(z80_state *z80); \
INLINE void prefix##_ec(z80_state *z80); INLINE void prefix##_ed(z80_state *z80); INLINE void prefix##_ee(z80_state *z80); INLINE void prefix##_ef(z80_state *z80); \
INLINE void prefix##_f0(z80_state *z80); INLINE void prefix##_f1(z80_state *z80); INLINE void prefix##_f2(z80_state *z80); INLINE void prefix##_f3(z80_state *z80); \
INLINE void prefix##_f4(z80_state *z80); INLINE void prefix##_f5(z80_state *z80); INLINE void prefix##_f6(z80_state *z80); INLINE void prefix##_f7(z80_state *z80); \
INLINE void prefix##_f8(z80_state *z80); INLINE void prefix##_f9(z80_state *z80); INLINE void prefix##_fa(z80_state *z80); INLINE void prefix##_fb(z80_state *z80); \
INLINE void prefix##_fc(z80_state *z80); INLINE void prefix##_fd(z80_state *z80); INLINE void prefix##_fe(z80_state *z80); INLINE void prefix##_ff(z80_state *z80); \
static const funcptr tablename[0x100] = {	\
prefix##_00,prefix##_01,prefix##_02,prefix##_03,prefix##_04,prefix##_05,prefix##_06,prefix##_07, \
prefix##_08,prefix##_09,prefix##_0a,prefix##_0b,prefix##_0c,prefix##_0d,prefix##_0e,prefix##_0f, \
prefix##_10,prefix##_11,prefix##_12,prefix##_13,prefix##_14,prefix##_15,prefix##_16,prefix##_17, \
prefix##_18,prefix##_19,prefix##_1a,prefix##_1b,prefix##_1c,prefix##_1d,prefix##_1e,prefix##_1f, \
prefix##_20,prefix##_21,prefix##_22,prefix##_23,prefix##_24,prefix##_25,prefix##_26,prefix##_27, \
prefix##_28,prefix##_29,prefix##_2a,prefix##_2b,prefix##_2c,prefix##_2d,prefix##_2e,prefix##_2f, \
prefix##_30,prefix##_31,prefix##_32,prefix##_33,prefix##_34,prefix##_35,prefix##_36,prefix##_37, \
prefix##_38,prefix##_39,prefix##_3a,prefix##_3b,prefix##_3c,prefix##_3d,prefix##_3e,prefix##_3f, \
prefix##_40,prefix##_41,prefix##_42,prefix##_43,prefix##_44,prefix##_45,prefix##_46,prefix##_47, \
prefix##_48,prefix##_49,prefix##_4a,prefix##_4b,prefix##_4c,prefix##_4d,prefix##_4e,prefix##_4f, \
prefix##_50,prefix##_51,prefix##_52,prefix##_53,prefix##_54,prefix##_55,prefix##_56,prefix##_57, \
prefix##_58,prefix##_59,prefix##_5a,prefix##_5b,prefix##_5c,prefix##_5d,prefix##_5e,prefix##_5f, \
prefix##_60,prefix##_61,prefix##_62,prefix##_63,prefix##_64,prefix##_65,prefix##_66,prefix##_67, \
prefix##_68,prefix##_69,prefix##_6a,prefix##_6b,prefix##_6c,prefix##_6d,prefix##_6e,prefix##_6f, \
prefix##_70,prefix##_71,prefix##_72,prefix##_73,prefix##_74,prefix##_75,prefix##_76,prefix##_77, \
prefix##_78,prefix##_79,prefix##_7a,prefix##_7b,prefix##_7c,prefix##_7d,prefix##_7e,prefix##_7f, \
prefix##_80,prefix##_81,prefix##_82,prefix##_83,prefix##_84,prefix##_85,prefix##_86,prefix##_87, \
prefix##_88,prefix##_89,prefix##_8a,prefix##_8b,prefix##_8c,prefix##_8d,prefix##_8e,prefix##_8f, \
prefix##_90,prefix##_91,prefix##_92,prefix##_93,prefix##_94,prefix##_95,prefix##_96,prefix##_97, \
prefix##_98,prefix##_99,prefix##_9a,prefix##_9b,prefix##_9c,prefix##_9d,prefix##_9e,prefix##_9f, \
prefix##_a0,prefix##_a1,prefix##_a2,prefix##_a3,prefix##_a4,prefix##_a5,prefix##_a6,prefix##_a7, \
prefix##_a8,prefix##_a9,prefix##_aa,prefix##_ab,prefix##_ac,prefix##_ad,prefix##_ae,prefix##_af, \
prefix##_b0,prefix##_b1,prefix##_b2,prefix##_b3,prefix##_b4,prefix##_b5,prefix##_b6,prefix##_b7, \
prefix##_b8,prefix##_b9,prefix##_ba,prefix##_bb,prefix##_bc,prefix##_bd,prefix##_be,prefix##_bf, \
prefix##_c0,prefix##_c1,prefix##_c2,prefix##_c3,prefix##_c4,prefix##_c5,prefix##_c6,prefix##_c7, \
prefix##_c8,prefix##_c9,prefix##_ca,prefix##_cb,prefix##_cc,prefix##_cd,prefix##_ce,prefix##_cf, \
prefix##_d0,prefix##_d1,prefix##_d2,prefix##_d3,prefix##_d4,prefix##_d5,prefix##_d6,prefix##_d7, \
prefix##_d8,prefix##_d9,prefix##_da,prefix##_db,prefix##_dc,prefix##_dd,prefix##_de,prefix##_df, \
prefix##_e0,prefix##_e1,prefix##_e2,prefix##_e3,prefix##_e4,prefix##_e5,prefix##_e6,prefix##_e7, \
prefix##_e8,prefix##_e9,prefix##_ea,prefix##_eb,prefix##_ec,prefix##_ed,prefix##_ee,prefix##_ef, \
prefix##_f0,prefix##_f1,prefix##_f2,prefix##_f3,prefix##_f4,prefix##_f5,prefix##_f6,prefix##_f7, \
prefix##_f8,prefix##_f9,prefix##_fa,prefix##_fb,prefix##_fc,prefix##_fd,prefix##_fe,prefix##_ff  \
}

PROTOTYPES(Z80op,op);
PROTOTYPES(Z80cb,cb);
PROTOTYPES(Z80dd,dd);
PROTOTYPES(Z80ed,ed);
PROTOTYPES(Z80fd,fd);
PROTOTYPES(Z80xycb,xycb);

/****************************************************************************/
/* Burn an odd amount of cycles, that is instructions taking something      */
/* different from 4 T-states per opcode (and r increment)                   */
/****************************************************************************/
INLINE void BURNODD(z80_state *z80, int cycles, int opcodes, int cyclesum)
{
	if( cycles > 0 )
	{
		z80->r += (cycles / cyclesum) * opcodes;
		z80->icount -= (cycles / cyclesum) * cyclesum;
	}
}

/***************************************************************
 * define an opcode function
 ***************************************************************/
#define OP(prefix,opcode)  INLINE void prefix##_##opcode(z80_state *z80)

/***************************************************************
 * adjust cycle count by n T-states
 ***************************************************************/
#define CC(Z,prefix,opcode)	do { (Z)->icount -= z80->cc_##prefix[opcode]; } while (0)

/***************************************************************
 * execute an opcode
 ***************************************************************/
#define EXEC(Z,prefix,opcode)								\
{															\
unsigned op = opcode;										\
CC(Z,prefix,op);											\
(*Z80##prefix[op])(Z);										\
}

#if BIG_SWITCH
#define EXEC_INLINE(Z,prefix,opcode)						\
{															\
unsigned op = opcode;										\
CC(Z,prefix,op);											\
switch(op)													\
{															\
case 0x00:prefix##_##00(Z);break; case 0x01:prefix##_##01(Z);break; case 0x02:prefix##_##02(Z);break; case 0x03:prefix##_##03(Z);break; \
case 0x04:prefix##_##04(Z);break; case 0x05:prefix##_##05(Z);break; case 0x06:prefix##_##06(Z);break; case 0x07:prefix##_##07(Z);break; \
case 0x08:prefix##_##08(Z);break; case 0x09:prefix##_##09(Z);break; case 0x0a:prefix##_##0a(Z);break; case 0x0b:prefix##_##0b(Z);break; \
case 0x0c:prefix##_##0c(Z);break; case 0x0d:prefix##_##0d(Z);break; case 0x0e:prefix##_##0e(Z);break; case 0x0f:prefix##_##0f(Z);break; \
case 0x10:prefix##_##10(Z);break; case 0x11:prefix##_##11(Z);break; case 0x12:prefix##_##12(Z);break; case 0x13:prefix##_##13(Z);break; \
case 0x14:prefix##_##14(Z);break; case 0x15:prefix##_##15(Z);break; case 0x16:prefix##_##16(Z);break; case 0x17:prefix##_##17(Z);break; \
case 0x18:prefix##_##18(Z);break; case 0x19:prefix##_##19(Z);break; case 0x1a:prefix##_##1a(Z);break; case 0x1b:prefix##_##1b(Z);break; \
case 0x1c:prefix##_##1c(Z);break; case 0x1d:prefix##_##1d(Z);break; case 0x1e:prefix##_##1e(Z);break; case 0x1f:prefix##_##1f(Z);break; \
case 0x20:prefix##_##20(Z);break; case 0x21:prefix##_##21(Z);break; case 0x22:prefix##_##22(Z);break; case 0x23:prefix##_##23(Z);break; \
case 0x24:prefix##_##24(Z);break; case 0x25:prefix##_##25(Z);break; case 0x26:prefix##_##26(Z);break; case 0x27:prefix##_##27(Z);break; \
case 0x28:prefix##_##28(Z);break; case 0x29:prefix##_##29(Z);break; case 0x2a:prefix##_##2a(Z);break; case 0x2b:prefix##_##2b(Z);break; \
case 0x2c:prefix##_##2c(Z);break; case 0x2d:prefix##_##2d(Z);break; case 0x2e:prefix##_##2e(Z);break; case 0x2f:prefix##_##2f(Z);break; \
case 0x30:prefix##_##30(Z);break; case 0x31:prefix##_##31(Z);break; case 0x32:prefix##_##32(Z);break; case 0x33:prefix##_##33(Z);break; \
case 0x34:prefix##_##34(Z);break; case 0x35:prefix##_##35(Z);break; case 0x36:prefix##_##36(Z);break; case 0x37:prefix##_##37(Z);break; \
case 0x38:prefix##_##38(Z);break; case 0x39:prefix##_##39(Z);break; case 0x3a:prefix##_##3a(Z);break; case 0x3b:prefix##_##3b(Z);break; \
case 0x3c:prefix##_##3c(Z);break; case 0x3d:prefix##_##3d(Z);break; case 0x3e:prefix##_##3e(Z);break; case 0x3f:prefix##_##3f(Z);break; \
case 0x40:prefix##_##40(Z);break; case 0x41:prefix##_##41(Z);break; case 0x42:prefix##_##42(Z);break; case 0x43:prefix##_##43(Z);break; \
case 0x44:prefix##_##44(Z);break; case 0x45:prefix##_##45(Z);break; case 0x46:prefix##_##46(Z);break; case 0x47:prefix##_##47(Z);break; \
case 0x48:prefix##_##48(Z);break; case 0x49:prefix##_##49(Z);break; case 0x4a:prefix##_##4a(Z);break; case 0x4b:prefix##_##4b(Z);break; \
case 0x4c:prefix##_##4c(Z);break; case 0x4d:prefix##_##4d(Z);break; case 0x4e:prefix##_##4e(Z);break; case 0x4f:prefix##_##4f(Z);break; \
case 0x50:prefix##_##50(Z);break; case 0x51:prefix##_##51(Z);break; case 0x52:prefix##_##52(Z);break; case 0x53:prefix##_##53(Z);break; \
case 0x54:prefix##_##54(Z);break; case 0x55:prefix##_##55(Z);break; case 0x56:prefix##_##56(Z);break; case 0x57:prefix##_##57(Z);break; \
case 0x58:prefix##_##58(Z);break; case 0x59:prefix##_##59(Z);break; case 0x5a:prefix##_##5a(Z);break; case 0x5b:prefix##_##5b(Z);break; \
case 0x5c:prefix##_##5c(Z);break; case 0x5d:prefix##_##5d(Z);break; case 0x5e:prefix##_##5e(Z);break; case 0x5f:prefix##_##5f(Z);break; \
case 0x60:prefix##_##60(Z);break; case 0x61:prefix##_##61(Z);break; case 0x62:prefix##_##62(Z);break; case 0x63:prefix##_##63(Z);break; \
case 0x64:prefix##_##64(Z);break; case 0x65:prefix##_##65(Z);break; case 0x66:prefix##_##66(Z);break; case 0x67:prefix##_##67(Z);break; \
case 0x68:prefix##_##68(Z);break; case 0x69:prefix##_##69(Z);break; case 0x6a:prefix##_##6a(Z);break; case 0x6b:prefix##_##6b(Z);break; \
case 0x6c:prefix##_##6c(Z);break; case 0x6d:prefix##_##6d(Z);break; case 0x6e:prefix##_##6e(Z);break; case 0x6f:prefix##_##6f(Z);break; \
case 0x70:prefix##_##70(Z);break; case 0x71:prefix##_##71(Z);break; case 0x72:prefix##_##72(Z);break; case 0x73:prefix##_##73(Z);break; \
case 0x74:prefix##_##74(Z);break; case 0x75:prefix##_##75(Z);break; case 0x76:prefix##_##76(Z);break; case 0x77:prefix##_##77(Z);break; \
case 0x78:prefix##_##78(Z);break; case 0x79:prefix##_##79(Z);break; case 0x7a:prefix##_##7a(Z);break; case 0x7b:prefix##_##7b(Z);break; \
case 0x7c:prefix##_##7c(Z);break; case 0x7d:prefix##_##7d(Z);break; case 0x7e:prefix##_##7e(Z);break; case 0x7f:prefix##_##7f(Z);break; \
case 0x80:prefix##_##80(Z);break; case 0x81:prefix##_##81(Z);break; case 0x82:prefix##_##82(Z);break; case 0x83:prefix##_##83(Z);break; \
case 0x84:prefix##_##84(Z);break; case 0x85:prefix##_##85(Z);break; case 0x86:prefix##_##86(Z);break; case 0x87:prefix##_##87(Z);break; \
case 0x88:prefix##_##88(Z);break; case 0x89:prefix##_##89(Z);break; case 0x8a:prefix##_##8a(Z);break; case 0x8b:prefix##_##8b(Z);break; \
case 0x8c:prefix##_##8c(Z);break; case 0x8d:prefix##_##8d(Z);break; case 0x8e:prefix##_##8e(Z);break; case 0x8f:prefix##_##8f(Z);break; \
case 0x90:prefix##_##90(Z);break; case 0x91:prefix##_##91(Z);break; case 0x92:prefix##_##92(Z);break; case 0x93:prefix##_##93(Z);break; \
case 0x94:prefix##_##94(Z);break; case 0x95:prefix##_##95(Z);break; case 0x96:prefix##_##96(Z);break; case 0x97:prefix##_##97(Z);break; \
case 0x98:prefix##_##98(Z);break; case 0x99:prefix##_##99(Z);break; case 0x9a:prefix##_##9a(Z);break; case 0x9b:prefix##_##9b(Z);break; \
case 0x9c:prefix##_##9c(Z);break; case 0x9d:prefix##_##9d(Z);break; case 0x9e:prefix##_##9e(Z);break; case 0x9f:prefix##_##9f(Z);break; \
case 0xa0:prefix##_##a0(Z);break; case 0xa1:prefix##_##a1(Z);break; case 0xa2:prefix##_##a2(Z);break; case 0xa3:prefix##_##a3(Z);break; \
case 0xa4:prefix##_##a4(Z);break; case 0xa5:prefix##_##a5(Z);break; case 0xa6:prefix##_##a6(Z);break; case 0xa7:prefix##_##a7(Z);break; \
case 0xa8:prefix##_##a8(Z);break; case 0xa9:prefix##_##a9(Z);break; case 0xaa:prefix##_##aa(Z);break; case 0xab:prefix##_##ab(Z);break; \
case 0xac:prefix##_##ac(Z);break; case 0xad:prefix##_##ad(Z);break; case 0xae:prefix##_##ae(Z);break; case 0xaf:prefix##_##af(Z);break; \
case 0xb0:prefix##_##b0(Z);break; case 0xb1:prefix##_##b1(Z);break; case 0xb2:prefix##_##b2(Z);break; case 0xb3:prefix##_##b3(Z);break; \
case 0xb4:prefix##_##b4(Z);break; case 0xb5:prefix##_##b5(Z);break; case 0xb6:prefix##_##b6(Z);break; case 0xb7:prefix##_##b7(Z);break; \
case 0xb8:prefix##_##b8(Z);break; case 0xb9:prefix##_##b9(Z);break; case 0xba:prefix##_##ba(Z);break; case 0xbb:prefix##_##bb(Z);break; \
case 0xbc:prefix##_##bc(Z);break; case 0xbd:prefix##_##bd(Z);break; case 0xbe:prefix##_##be(Z);break; case 0xbf:prefix##_##bf(Z);break; \
case 0xc0:prefix##_##c0(Z);break; case 0xc1:prefix##_##c1(Z);break; case 0xc2:prefix##_##c2(Z);break; case 0xc3:prefix##_##c3(Z);break; \
case 0xc4:prefix##_##c4(Z);break; case 0xc5:prefix##_##c5(Z);break; case 0xc6:prefix##_##c6(Z);break; case 0xc7:prefix##_##c7(Z);break; \
case 0xc8:prefix##_##c8(Z);break; case 0xc9:prefix##_##c9(Z);break; case 0xca:prefix##_##ca(Z);break; case 0xcb:prefix##_##cb(Z);break; \
case 0xcc:prefix##_##cc(Z);break; case 0xcd:prefix##_##cd(Z);break; case 0xce:prefix##_##ce(Z);break; case 0xcf:prefix##_##cf(Z);break; \
case 0xd0:prefix##_##d0(Z);break; case 0xd1:prefix##_##d1(Z);break; case 0xd2:prefix##_##d2(Z);break; case 0xd3:prefix##_##d3(Z);break; \
case 0xd4:prefix##_##d4(Z);break; case 0xd5:prefix##_##d5(Z);break; case 0xd6:prefix##_##d6(Z);break; case 0xd7:prefix##_##d7(Z);break; \
case 0xd8:prefix##_##d8(Z);break; case 0xd9:prefix##_##d9(Z);break; case 0xda:prefix##_##da(Z);break; case 0xdb:prefix##_##db(Z);break; \
case 0xdc:prefix##_##dc(Z);break; case 0xdd:prefix##_##dd(Z);break; case 0xde:prefix##_##de(Z);break; case 0xdf:prefix##_##df(Z);break; \
case 0xe0:prefix##_##e0(Z);break; case 0xe1:prefix##_##e1(Z);break; case 0xe2:prefix##_##e2(Z);break; case 0xe3:prefix##_##e3(Z);break; \
case 0xe4:prefix##_##e4(Z);break; case 0xe5:prefix##_##e5(Z);break; case 0xe6:prefix##_##e6(Z);break; case 0xe7:prefix##_##e7(Z);break; \
case 0xe8:prefix##_##e8(Z);break; case 0xe9:prefix##_##e9(Z);break; case 0xea:prefix##_##ea(Z);break; case 0xeb:prefix##_##eb(Z);break; \
case 0xec:prefix##_##ec(Z);break; case 0xed:prefix##_##ed(Z);break; case 0xee:prefix##_##ee(Z);break; case 0xef:prefix##_##ef(Z);break; \
case 0xf0:prefix##_##f0(Z);break; case 0xf1:prefix##_##f1(Z);break; case 0xf2:prefix##_##f2(Z);break; case 0xf3:prefix##_##f3(Z);break; \
case 0xf4:prefix##_##f4(Z);break; case 0xf5:prefix##_##f5(Z);break; case 0xf6:prefix##_##f6(Z);break; case 0xf7:prefix##_##f7(Z);break; \
case 0xf8:prefix##_##f8(Z);break; case 0xf9:prefix##_##f9(Z);break; case 0xfa:prefix##_##fa(Z);break; case 0xfb:prefix##_##fb(Z);break; \
case 0xfc:prefix##_##fc(Z);break; case 0xfd:prefix##_##fd(Z);break; case 0xfe:prefix##_##fe(Z);break; case 0xff:prefix##_##ff(Z);break; \
}																																	\
}
#else
#define EXEC_INLINE EXEC
#endif


/***************************************************************
 * Enter halt state; write 1 to fake port on first execution
 ***************************************************************/
#define ENTER_HALT(Z) do {									\
(Z)->PC--;													\
(Z)->halt = 1;												\
if( (Z)->irq_state == CLEAR_LINE )							\
CPU_BURN_NAME(z80)( (Z)->device, (Z)->icount);				\
} while (0)

/***************************************************************
 * Leave halt state; write 0 to fake port
 ***************************************************************/
#define LEAVE_HALT(Z) do {									\
if( (Z)->halt )												\
{															\
(Z)->halt = 0;												\
(Z)->PC++;													\
}															\
} while (0)

/***************************************************************
 * Input a byte from given I/O port
 ***************************************************************/
#define IN(Z,port)  		memory_read_byte_8le((Z)->io, port)

/***************************************************************
 * Output a byte to given I/O port
 ***************************************************************/
#define OUT(Z,port,value)	memory_write_byte_8le((Z)->io, port, value)

/***************************************************************
 * Read a byte from given memory location
 ***************************************************************/
#define RM(Z,addr)			memory_read_byte_8le((Z)->program, addr)

/***************************************************************
 * Read a word from given memory location
 ***************************************************************/
INLINE void RM16(z80_state *z80, UINT32 addr, PAIR *r)
{
	r->b.l = RM(z80, addr);
	r->b.h = RM(z80, (addr+1)&0xffff);
}

/***************************************************************
 * Write a byte to given memory location
 ***************************************************************/
#define WM(Z,addr,value)	memory_write_byte_8le((Z)->program, addr, value)

/***************************************************************
 * Write a word to given memory location
 ***************************************************************/
INLINE void WM16(z80_state *z80, UINT32 addr, PAIR *r)
{
	WM(z80, addr, r->b.l);
	WM(z80, (addr+1)&0xffff, r->b.h);
}

/***************************************************************
 * ROP() is identical to RM() except it is used for
 * reading opcodes. In case of system with memory mapped I/O,
 * this function can be used to greatly speed up emulation
 ***************************************************************/
INLINE UINT8 ROP(z80_state *z80)
{
	unsigned pc = z80->PCD;
	z80->PC++;
	return memory_decrypted_read_byte(z80->program, pc);
}

/****************************************************************
 * ARG(z80) is identical to ROP() except it is used
 * for reading opcode arguments. This difference can be used to
 * support systems that use different encoding mechanisms for
 * opcodes and opcode arguments
 ***************************************************************/
INLINE UINT8 ARG(z80_state *z80)
{
	unsigned pc = z80->PCD;
	z80->PC++;
	return memory_raw_read_byte(z80->program, pc);
}

INLINE UINT32 ARG16(z80_state *z80)
{
	unsigned pc = z80->PCD;
	z80->PC += 2;
	return memory_raw_read_byte(z80->program, pc) | (memory_raw_read_byte(z80->program, (pc+1)&0xffff) << 8);
}

/***************************************************************
 * Calculate the effective address EA of an opcode using
 * IX+offset resp. IY+offset addressing.
 ***************************************************************/
#define EAX(Z)		do { (Z)->ea = (UINT32)(UINT16)((Z)->IX + (INT8)ARG(Z)); (Z)->WZ = (Z)->ea; } while (0)
#define EAY(Z)		do { (Z)->ea = (UINT32)(UINT16)((Z)->IY + (INT8)ARG(Z)); (Z)->WZ = (Z)->ea; } while (0)

/***************************************************************
 * POP
 ***************************************************************/
#define POP(Z,DR)	do { RM16((Z), (Z)->SPD, &(Z)->DR); (Z)->SP += 2; } while (0)

/***************************************************************
 * PUSH
 ***************************************************************/
#define PUSH(Z,SR)	do { (Z)->SP -= 2; WM16((Z), (Z)->SPD, &(Z)->SR); } while (0)

/***************************************************************
 * JP
 ***************************************************************/
#define JP(Z) do {											\
(Z)->PCD = ARG16(Z);										\
(Z)->WZ = (Z)->PCD;											\
} while (0)

/***************************************************************
 * JP_COND
 ***************************************************************/
#define JP_COND(Z, cond) do {								\
if (cond)													\
{															\
(Z)->PCD = ARG16(Z);										\
(Z)->WZ = (Z)->PCD;											\
}															\
else														\
{															\
(Z)->WZ = ARG16(Z); /* implicit do PC += 2 */				\
}															\
} while (0)

/***************************************************************
 * JR
 ***************************************************************/
#define JR(Z) do {											\
INT8 arg = (INT8)ARG(Z);	/* ARG() also increments PC */	\
(Z)->PC += arg;				/* so don't do PC += ARG() */	\
(Z)->WZ = (Z)->PC;											\
} while (0)

/***************************************************************
 * JR_COND
 ***************************************************************/
#define JR_COND(Z, cond, opcode) do {						\
if (cond)													\
{															\
JR(Z);														\
CC(Z, ex, opcode);											\
}															\
else (Z)->PC++;												\
} while (0)

/***************************************************************
 * CALL
 ***************************************************************/
#define CALL(Z) do {										\
(Z)->ea = ARG16(Z);											\
(Z)->WZ = (Z)->ea;											\
PUSH((Z), pc);												\
(Z)->PCD = (Z)->ea;											\
} while (0)

/***************************************************************
 * CALL_COND
 ***************************************************************/
#define CALL_COND(Z, cond, opcode) do {						\
if (cond)													\
{															\
(Z)->ea = ARG16(Z);											\
(Z)->WZ = (Z)->ea;											\
PUSH((Z), pc);												\
(Z)->PCD = (Z)->ea;											\
CC(Z, ex, opcode);											\
}															\
else														\
{															\
z80->WZ = ARG16(z80);  /* implicit call PC+=2;   */			\
}															\
} while (0)

/***************************************************************
 * RET_COND
 ***************************************************************/
#define RET_COND(Z, cond, opcode) do {						\
if (cond)													\
{															\
POP((Z), pc);												\
(Z)->WZ = (Z)->PC;											\
CC(Z, ex, opcode);											\
}															\
} while (0)

/***************************************************************
 * RETN
 ***************************************************************/
#define RETN(Z) do {										\
LOG(("Z80 '%s' RETN z80->iff1:%d z80->iff2:%d\n",			\
(Z)->device->tag, (Z)->iff1, (Z)->iff2));					\
POP((Z), pc);												\
(Z)->WZ = (Z)->PC;											\
(Z)->iff1 = (Z)->iff2;										\
} while (0)

/***************************************************************
 * RETI
 ***************************************************************/
#define RETI(Z) do {										\
POP((Z), pc);												\
(Z)->WZ = (Z)->PC;											\
/* according to http://www.msxnet.org/tech/z80-documented.pdf */\
(Z)->iff1 = (Z)->iff2;										\
if ((Z)->daisy != NULL)										\
z80daisy_call_reti_device((Z)->daisy);						\
} while (0)

/***************************************************************
 * LD   R,A
 ***************************************************************/
#define LD_R_A(Z) do {										\
(Z)->r = (Z)->A;											\
(Z)->r2 = (Z)->A & 0x80;			/* keep bit 7 of r */	\
} while (0)

/***************************************************************
 * LD   A,R
 ***************************************************************/
#define LD_A_R(Z) do {										\
(Z)->A = ((Z)->r & 0x7f) | (Z)->r2;							\
(Z)->F = ((Z)->F & CF) | SZ[(Z)->A] | ((Z)->iff2 << 2);		\
} while (0)

/***************************************************************
 * LD   I,A
 ***************************************************************/
#define LD_I_A(Z) do {										\
(Z)->i = (Z)->A;											\
} while (0)

/***************************************************************
 * LD   A,I
 ***************************************************************/
#define LD_A_I(Z) do {										\
(Z)->A = (Z)->i;											\
(Z)->F = ((Z)->F & CF) | SZ[(Z)->A] | ((Z)->iff2 << 2);		\
} while (0)

/***************************************************************
 * RST
 ***************************************************************/
#define RST(Z, addr) do {									\
PUSH((Z), pc);												\
(Z)->PCD = addr;											\
(Z)->WZ = (Z)->PC;											\
} while (0)

/***************************************************************
 * INC  r8
 ***************************************************************/
INLINE UINT8 INC(z80_state *z80, UINT8 value)
{
	UINT8 res = value + 1;
	z80->F = (z80->F & CF) | SZHV_inc[res];
	return (UINT8)res;
}

/***************************************************************
 * DEC  r8
 ***************************************************************/
INLINE UINT8 DEC(z80_state *z80, UINT8 value)
{
	UINT8 res = value - 1;
	z80->F = (z80->F & CF) | SZHV_dec[res];
	return res;
}

/***************************************************************
 * RLCA
 ***************************************************************/
#define RLCA(Z) do {										\
(Z)->A = ((Z)->A << 1) | ((Z)->A >> 7);						\
(Z)->F = ((Z)->F & (SF | ZF | PF)) | ((Z)->A & (YF | XF | CF)); \
} while (0)

/***************************************************************
 * RRCA
 ***************************************************************/
#define RRCA(Z) do {										\
(Z)->F = ((Z)->F & (SF | ZF | PF)) | ((Z)->A & CF);			\
(Z)->A = ((Z)->A >> 1) | ((Z)->A << 7);						\
(Z)->F |= ((Z)->A & (YF | XF));								\
} while (0)

/***************************************************************
 * RLA
 ***************************************************************/
#define RLA(Z) do {											\
UINT8 res = ((Z)->A << 1) | ((Z)->F & CF);					\
UINT8 c = ((Z)->A & 0x80) ? CF : 0;							\
(Z)->F = ((Z)->F & (SF | ZF | PF)) | c | (res & (YF | XF));	\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * RRA
 ***************************************************************/
#define RRA(Z) do {											\
UINT8 res = ((Z)->A >> 1) | ((Z)->F << 7);					\
UINT8 c = ((Z)->A & 0x01) ? CF : 0;							\
(Z)->F = ((Z)->F & (SF | ZF | PF)) | c | (res & (YF | XF));	\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * RRD
 ***************************************************************/
#define RRD(Z) do {											\
UINT8 n = RM((Z), (Z)->HL);									\
(Z)->WZ = (Z)->HL+1;										\
WM((Z), (Z)->HL, (n >> 4) | ((Z)->A << 4));					\
(Z)->A = ((Z)->A & 0xf0) | (n & 0x0f);						\
(Z)->F = ((Z)->F & CF) | SZP[(Z)->A];						\
} while (0)

/***************************************************************
 * RLD
 ***************************************************************/
#define RLD(Z) do {											\
UINT8 n = RM((Z), (Z)->HL);									\
(Z)->WZ = (Z)->HL+1;										\
WM((Z), (Z)->HL, (n << 4) | ((Z)->A & 0x0f));				\
(Z)->A = ((Z)->A & 0xf0) | (n >> 4);						\
(Z)->F = ((Z)->F & CF) | SZP[(Z)->A];						\
} while (0)

/***************************************************************
 * ADD  A,n
 ***************************************************************/
#define ADD(Z, value) do {									\
UINT32 ah = (Z)->AFD & 0xff00;								\
UINT32 res = (UINT8)((ah >> 8) + value);					\
(Z)->F = SZHVC_add[ah | res];								\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * ADC  A,n
 ***************************************************************/
#define ADC(Z, value) do {									\
UINT32 ah = (Z)->AFD & 0xff00, c = (Z)->AFD & 1;			\
UINT32 res = (UINT8)((ah >> 8) + value + c);				\
(Z)->F = SZHVC_add[(c << 16) | ah | res];					\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * SUB  n
 ***************************************************************/
#define SUB(Z, value) do {									\
UINT32 ah = (Z)->AFD & 0xff00;								\
UINT32 res = (UINT8)((ah >> 8) - value);					\
(Z)->F = SZHVC_sub[ah | res];								\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * SBC  A,n
 ***************************************************************/
#define SBC(Z, value) do {									\
UINT32 ah = (Z)->AFD & 0xff00, c = (Z)->AFD & 1;			\
UINT32 res = (UINT8)((ah >> 8) - value - c);				\
(Z)->F = SZHVC_sub[(c<<16) | ah | res];						\
(Z)->A = res;												\
} while (0)

/***************************************************************
 * NEG
 ***************************************************************/
#define NEG(Z) do {											\
UINT8 value = (Z)->A;										\
(Z)->A = 0;													\
SUB(Z, value);												\
} while (0)

/***************************************************************
 * DAA
 ***************************************************************/
#define DAA(Z) do {											\
UINT8 a = (Z)->A;											\
if ((Z)->F & NF) {											\
if (((Z)->F&HF) | (((Z)->A&0xf)>9)) a-=6;					\
if (((Z)->F&CF) | ((Z)->A>0x99)) a-=0x60;					\
}															\
else {														\
if (((Z)->F&HF) | (((Z)->A&0xf)>9)) a+=6;					\
if (((Z)->F&CF) | ((Z)->A>0x99)) a+=0x60;					\
}															\
\
(Z)->F = ((Z)->F&(CF|NF)) | ((Z)->A>0x99) | (((Z)->A^a)&HF) | SZP[a]; \
(Z)->A = a;													\
} while (0)

/***************************************************************
 * AND  n
 ***************************************************************/
#define AND(Z, value) do {									\
(Z)->A &= value;											\
(Z)->F = SZP[(Z)->A] | HF;									\
} while (0)

/***************************************************************
 * OR   n
 ***************************************************************/
#define OR(Z, value) do {									\
(Z)->A |= value;											\
(Z)->F = SZP[(Z)->A];										\
} while (0)

/***************************************************************
 * XOR  n
 ***************************************************************/
#define XOR(Z, value) do {									\
(Z)->A ^= value;											\
(Z)->F = SZP[(Z)->A];										\
} while (0)

/***************************************************************
 * CP   n
 ***************************************************************/
#define CP(Z, value) do {									\
unsigned val = value;										\
UINT32 ah = (Z)->AFD & 0xff00;								\
UINT32 res = (UINT8)((ah >> 8) - val);						\
(Z)->F = (SZHVC_sub[ah | res] & ~(YF | XF)) |				\
(val & (YF | XF));											\
} while (0)

/***************************************************************
 * EX   AF,AF'
 ***************************************************************/
#define EX_AF(Z) do {											\
PAIR tmp;													\
tmp = (Z)->af; (Z)->af = (Z)->af2; (Z)->af2 = tmp;			\
} while (0)

/***************************************************************
 * EX   DE,HL
 ***************************************************************/
#define EX_DE_HL(Z) do {									\
PAIR tmp;													\
tmp = (Z)->de; (Z)->de = (Z)->hl; (Z)->hl = tmp;			\
} while (0)

/***************************************************************
 * EXX
 ***************************************************************/
#define EXX(Z) do {											\
PAIR tmp;													\
tmp = (Z)->bc; (Z)->bc = (Z)->bc2; (Z)->bc2 = tmp;			\
tmp = (Z)->de; (Z)->de = (Z)->de2; (Z)->de2 = tmp;			\
tmp = (Z)->hl; (Z)->hl = (Z)->hl2; (Z)->hl2 = tmp;			\
} while (0)

/***************************************************************
 * EX   (SP),r16
 ***************************************************************/
#define EXSP(Z, DR) do {									\
PAIR tmp = { { 0, 0, 0, 0 } };								\
RM16((Z), (Z)->SPD, &tmp);									\
WM16((Z), (Z)->SPD, &(Z)->DR);								\
(Z)->DR = tmp;												\
(Z)->WZ = (Z)->DR.d;										\
} while (0)

/***************************************************************
 * ADD16
 ***************************************************************/
#define ADD16(Z, DR, SR) do {								\
UINT32 res = (Z)->DR.d + (Z)->SR.d;							\
(Z)->WZ = (Z)->DR.d + 1;									\
(Z)->F = ((Z)->F & (SF | ZF | VF)) |						\
((((Z)->DR.d ^ res ^ (Z)->SR.d) >> 8) & HF) |				\
((res >> 16) & CF) | ((res >> 8) & (YF | XF));				\
(Z)->DR.w.l = (UINT16)res;									\
} while (0)

/***************************************************************
 * ADC  r16,r16
 ***************************************************************/
#define ADC16(Z, Reg) do {									\
UINT32 res = (Z)->HLD + (Z)->Reg.d + ((Z)->F & CF);			\
(Z)->WZ = (Z)->HL + 1;										\
(Z)->F = ((((Z)->HLD ^ res ^ (Z)->Reg.d) >> 8) & HF) |		\
((res >> 16) & CF) |										\
((res >> 8) & (SF | YF | XF)) |								\
((res & 0xffff) ? 0 : ZF) |									\
((((Z)->Reg.d ^ (Z)->HLD ^ 0x8000) & ((Z)->Reg.d ^ res) & 0x8000) >> 13); \
(Z)->HL = (UINT16)res;										\
} while (0)

/***************************************************************
 * SBC  r16,r16
 ***************************************************************/
#define SBC16(Z, Reg) do {									\
UINT32 res = (Z)->HLD - (Z)->Reg.d - ((Z)->F & CF);			\
(Z)->WZ = (Z)->HL + 1;										\
(Z)->F = ((((Z)->HLD ^ res ^ (Z)->Reg.d) >> 8) & HF) | NF |	\
((res >> 16) & CF) |										\
((res >> 8) & (SF | YF | XF)) |								\
((res & 0xffff) ? 0 : ZF) |									\
((((Z)->Reg.d ^ (Z)->HLD) & ((Z)->HLD ^ res) &0x8000) >> 13); \
(Z)->HL = (UINT16)res;										\
} while (0)

/***************************************************************
 * RLC  r8
 ***************************************************************/
INLINE UINT8 RLC(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (res >> 7)) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RRC  r8
 ***************************************************************/
INLINE UINT8 RRC(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res << 7)) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RL   r8
 ***************************************************************/
INLINE UINT8 RL(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | (z80->F & CF)) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * RR   r8
 ***************************************************************/
INLINE UINT8 RR(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (z80->F << 7)) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLA  r8
 ***************************************************************/
INLINE UINT8 SLA(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = (res << 1) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRA  r8
 ***************************************************************/
INLINE UINT8 SRA(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = ((res >> 1) | (res & 0x80)) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SLL  r8
 ***************************************************************/
INLINE UINT8 SLL(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x80) ? CF : 0;
	res = ((res << 1) | 0x01) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * SRL  r8
 ***************************************************************/
INLINE UINT8 SRL(z80_state *z80, UINT8 value)
{
	unsigned res = value;
	unsigned c = (res & 0x01) ? CF : 0;
	res = (res >> 1) & 0xff;
	z80->F = SZP[res] | c;
	return res;
}

/***************************************************************
 * BIT  bit,r8
 ***************************************************************/
#undef BIT
#define BIT(Z, bit, reg) do {								\
(Z)->F = ((Z)->F & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | (reg & (YF|XF)); \
} while (0)

/***************************************************************
 * BIT  bit,(HL)
 ***************************************************************/
#define BIT_HL(Z, bit, reg) do {							\
(Z)->F = ((Z)->F & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | ((Z)->WZ_H & (YF|XF)); \
} while (0)

/***************************************************************
 * BIT  bit,(IX/Y+o)
 ***************************************************************/
#define BIT_XY(Z, bit, reg) do {							\
(Z)->F = ((Z)->F & CF) | HF | (SZ_BIT[reg & (1<<bit)] & ~(YF|XF)) | (((Z)->ea>>8) & (YF|XF)); \
} while (0)

/***************************************************************
 * RES  bit,r8
 ***************************************************************/
INLINE UINT8 RES(UINT8 bit, UINT8 value)
{
	return value & ~(1<<bit);
}

/***************************************************************
 * SET  bit,r8
 ***************************************************************/
INLINE UINT8 SET(UINT8 bit, UINT8 value)
{
	return value | (1<<bit);
}

/***************************************************************
 * LDI
 ***************************************************************/
#define LDI(Z) do {											\
UINT8 io = RM((Z), (Z)->HL);								\
WM((Z), (Z)->DE, io);										\
(Z)->F &= SF | ZF | CF;										\
if (((Z)->A + io) & 0x02) (Z)->F |= YF; /* bit 1 -> flag 5 */ \
if (((Z)->A + io) & 0x08) (Z)->F |= XF; /* bit 3 -> flag 3 */ \
(Z)->HL++; (Z)->DE++; (Z)->BC--;							\
if((Z)->BC) (Z)->F |= VF;									\
} while (0)

/***************************************************************
 * CPI
 ***************************************************************/
#define CPI(Z) do {											\
UINT8 val = RM((Z), (Z)->HL);								\
UINT8 res = (Z)->A - val;									\
(Z)->WZ++;													\
(Z)->HL++; (Z)->BC--;										\
(Z)->F = ((Z)->F & CF) | (SZ[res]&~(YF|XF)) | (((Z)->A^val^res)&HF) | NF; \
if ((Z)->F & HF) res -= 1;									\
if (res & 0x02) (Z)->F |= YF; /* bit 1 -> flag 5 */			\
if (res & 0x08) (Z)->F |= XF; /* bit 3 -> flag 3 */			\
if ((Z)->BC) (Z)->F |= VF;									\
} while (0)

/***************************************************************
 * INI
 ***************************************************************/
#define INI(Z) do {											\
unsigned t;													\
UINT8 io = IN((Z), (Z)->BC);								\
(Z)->WZ = (Z)->BC + 1;										\
(Z)->B--;													\
WM((Z), (Z)->HL, io);										\
(Z)->HL++;													\
(Z)->F = SZ[(Z)->B];										\
t = (unsigned)(((Z)->C + 1) & 0xff) + (unsigned)io;			\
if (io & SF) (Z)->F |= NF;									\
if (t & 0x100) (Z)->F |= HF | CF;							\
(Z)->F |= SZP[(UINT8)(t & 0x07) ^ (Z)->B] & PF;				\
} while (0)

/***************************************************************
 * OUTI
 ***************************************************************/
#define OUTI(Z) do {										\
unsigned t;													\
UINT8 io = RM((Z), (Z)->HL);								\
(Z)->B--;													\
(Z)->WZ = (Z)->BC + 1;										\
OUT((Z), (Z)->BC, io);										\
(Z)->HL++;													\
(Z)->F = SZ[(Z)->B];										\
t = (unsigned)(Z)->L + (unsigned)io;						\
if (io & SF) (Z)->F |= NF;									\
if (t & 0x100) (Z)->F |= HF | CF;							\
(Z)->F |= SZP[(UINT8)(t & 0x07) ^ (Z)->B] & PF;				\
} while (0)

/***************************************************************
 * LDD
 ***************************************************************/
#define LDD(Z) do {											\
UINT8 io = RM((Z), (Z)->HL);								\
WM((Z), (Z)->DE, io);										\
(Z)->F &= SF | ZF | CF;										\
if (((Z)->A + io) & 0x02) (Z)->F |= YF; /* bit 1 -> flag 5 */ \
if (((Z)->A + io) & 0x08) (Z)->F |= XF; /* bit 3 -> flag 3 */ \
(Z)->HL--; (Z)->DE--; (Z)->BC--;							\
if ((Z)->BC) (Z)->F |= VF;									\
} while (0)

/***************************************************************
 * CPD
 ***************************************************************/
#define CPD(Z) do {											\
UINT8 val = RM((Z), (Z)->HL);								\
UINT8 res = (Z)->A - val;									\
(Z)->WZ--;													\
(Z)->HL--; (Z)->BC--;										\
(Z)->F = ((Z)->F & CF) | (SZ[res]&~(YF|XF)) | (((Z)->A^val^res)&HF) | NF; \
if ((Z)->F & HF) res -= 1;									\
if (res & 0x02) (Z)->F |= YF; /* bit 1 -> flag 5 */			\
if (res & 0x08) (Z)->F |= XF; /* bit 3 -> flag 3 */			\
if ((Z)->BC) (Z)->F |= VF;									\
} while (0)

/***************************************************************
 * IND
 ***************************************************************/
#define IND(Z) do {											\
unsigned t;													\
UINT8 io = IN((Z), (Z)->BC);								\
(Z)->WZ = (Z)->BC - 1;										\
(Z)->B--;													\
WM((Z), (Z)->HL, io);										\
(Z)->HL--;													\
(Z)->F = SZ[(Z)->B];										\
t = ((unsigned)((Z)->C - 1) & 0xff) + (unsigned)io;			\
if (io & SF) (Z)->F |= NF;									\
if (t & 0x100) (Z)->F |= HF | CF;							\
(Z)->F |= SZP[(UINT8)(t & 0x07) ^ (Z)->B] & PF;				\
} while (0)

/***************************************************************
 * OUTD
 ***************************************************************/
#define OUTD(Z) do {										\
unsigned t;													\
UINT8 io = RM((Z), (Z)->HL);								\
(Z)->B--;													\
(Z)->WZ = (Z)->BC - 1;										\
OUT((Z), (Z)->BC, io);										\
(Z)->HL--;													\
(Z)->F = SZ[(Z)->B];										\
t = (unsigned)(Z)->L + (unsigned)io;						\
if (io & SF) (Z)->F |= NF;									\
if (t & 0x100) (Z)->F |= HF | CF;							\
(Z)->F |= SZP[(UINT8)(t & 0x07) ^ (Z)->B] & PF;				\
} while (0)

/***************************************************************
 * LDIR
 ***************************************************************/
#define LDIR(Z) do {										\
LDI(Z);														\
if ((Z)->BC != 0)											\
{															\
(Z)->PC -= 2;												\
(Z)->WZ = (Z)->PC + 1;										\
CC(Z, ex, 0xb0);											\
}															\
} while (0)

/***************************************************************
 * CPIR
 ***************************************************************/
#define CPIR(Z) do {										\
CPI(Z);														\
if ((Z)->BC != 0 && !((Z)->F & ZF))							\
{															\
(Z)->PC -= 2;												\
(Z)->WZ = (Z)->PC + 1;										\
CC(Z, ex, 0xb1);											\
}															\
} while (0)

/***************************************************************
 * INIR
 ***************************************************************/
#define INIR(Z) do {										\
INI(Z);														\
if ((Z)->B != 0)											\
{															\
(Z)->PC -= 2;												\
CC(Z, ex, 0xb2);											\
}															\
} while (0)

/***************************************************************
 * OTIR
 ***************************************************************/
#define OTIR(Z) do {										\
OUTI(Z);													\
if ((Z)->B != 0)											\
{															\
(Z)->PC -= 2;												\
CC(Z, ex, 0xb3);											\
}															\
} while (0)

/***************************************************************
 * LDDR
 ***************************************************************/
#define LDDR(Z) do {										\
LDD(Z);														\
if ((Z)->BC != 0)											\
{															\
(Z)->PC -= 2;												\
(Z)->WZ = (Z)->PC + 1;										\
CC(Z, ex, 0xb8);											\
}															\
} while (0)

/***************************************************************
 * CPDR
 ***************************************************************/
#define CPDR(Z) do {										\
CPD(Z);														\
if ((Z)->BC != 0 && !((Z)->F & ZF))							\
{															\
(Z)->PC -= 2;												\
(Z)->WZ = (Z)->PC + 1;										\
CC(Z, ex, 0xb9);											\
}															\
} while (0)

/***************************************************************
 * INDR
 ***************************************************************/
#define INDR(Z) do {										\
IND(Z);														\
if ((Z)->B != 0)											\
{															\
(Z)->PC -= 2;												\
CC(Z, ex, 0xba);											\
}															\
} while (0)

/***************************************************************
 * OTDR
 ***************************************************************/
#define OTDR(Z) do {										\
OUTD(Z);													\
if ((Z)->B != 0)											\
{															\
(Z)->PC -= 2;												\
CC(Z, ex, 0xbb);											\
}															\
} while (0)

/***************************************************************
 * EI
 ***************************************************************/
#define EI(Z) do {											\
(Z)->iff1 = (Z)->iff2 = 1;									\
(Z)->after_ei = TRUE;										\
} while (0)
