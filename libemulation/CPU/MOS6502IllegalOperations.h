
/**
 * libemulation
 * MOS6502IllegalOperations
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS6502 illegal operations.
 */

/*****************************************************************************
 *
 *   ill02.h
 *   Addressing mode and opcode macros for the NMOS 6502 illegal opcodes
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
 *   6502 core Copyright Peter Trauner, all rights reserved.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     pullmoll@t-online.de
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *****************************************************************************/

/* test with the excellent C64 Emulator test suite
 ? at www.funet.fi/pub/cbm/documents/chipdata/tsuit215.zip
 good reference in the vice emulator (source) distribution doc/64doc.txt
 
 $ab=OAL like in 6502-NMOS.extra.opcodes, vice so in vice (lxa)
 */

/***************************************************************
 ***************************************************************
 *          Macros to emulate the 6502 opcodes
 ***************************************************************
 ***************************************************************/

/***************************************************************
 *  ANC logical and, set carry from bit of A
 ***************************************************************/
#define ANC 												\
P &= ~F_C;													\
A = (OEUInt8)(A & tmp);										\
if (A & 0x80)												\
P |= F_C;													\
SET_NZ(A)

/***************************************************************
 *  ASR logical and, logical shift right
 ***************************************************************/
#define ASR 												\
tmp &= A;													\
LSR

/***************************************************************
 * AST  and stack; transfer to accumulator and index X
 * logical and stack (LSB) with data, transfer result to S
 * transfer result to accumulator and index X also
 ***************************************************************/
#define AST 												\
S &= tmp;													\
A = X = S;													\
SET_NZ(A)

/***************************************************************
 *  ARR logical and, rotate right
 ***************************************************************/
#define ARR 												\
if( P & F_D )												\
{															\
int lo, hi, t;												\
tmp &= A;													\
t = tmp;													\
hi = tmp &0xf0;												\
lo = tmp &0x0f;												\
if( P & F_C )												\
{															\
tmp = (tmp >> 1) | 0x80;									\
P |= F_N;													\
}															\
else														\
{															\
tmp >>= 1;													\
P &= ~F_N;													\
}															\
if( tmp )													\
P &= ~F_Z;													\
else														\
P |= F_Z;													\
if( (t^tmp) & 0x40 )										\
P|=F_V;														\
else														\
P &= ~F_V;													\
if( lo + (lo & 0x01) > 0x05 )								\
tmp = (tmp & 0xf0) | ((tmp+6) & 0xf);						\
if( hi + (hi & 0x10) > 0x50 )								\
{															\
P |= F_C;													\
tmp = (tmp+0x60) & 0xff;									\
}															\
else														\
P &= ~F_C;													\
}															\
else														\
{															\
tmp &= A;													\
ROR;														\
P &=~(F_V|F_C);												\
if( tmp & 0x40 )											\
P|=F_C;														\
if( (tmp & 0x60) == 0x20 || (tmp & 0x60) == 0x40 )			\
P|=F_V;														\
}

/***************************************************************
 *  ASX logical and X w/ A, subtract data from X
 ***************************************************************/
#define ASX 												\
P &= ~F_C;													\
X &= A; 													\
if (X >= tmp)												\
P |= F_C;													\
X = (OEUInt8)(X - tmp);										\
SET_NZ(X)

/***************************************************************
 *  AXA transfer index X to accumulator, logical and
 * depends on the data of the dma device (videochip) fetched
 * between opcode read and operand read
 ***************************************************************/
#define AXA 												\
A = (OEUInt8)( (A|0xee)& X & tmp);							\
SET_NZ(A)

/***************************************************************
 *  DCP decrement data and compare
 ***************************************************************/
#define DCP 												\
tmp = (OEUInt8)(tmp-1);										\
P &= ~F_C;													\
if (A >= tmp)												\
P |= F_C;													\
SET_NZ((OEUInt8)(A - tmp))

/***************************************************************
 *  DOP double no operation
 ***************************************************************/
//#define DOP 												\
//RDOPARG()

/***************************************************************
 *  ISB increment and subtract with carry
 ***************************************************************/
#define ISB 												\
tmp = (OEUInt8)(tmp+1);										\
SBC

/***************************************************************
 *  LAX load accumulator and index X
 ***************************************************************/
#define LAX 												\
A = X = (OEUInt8)tmp; 										\
SET_NZ(A)

/***************************************************************
 *  OAL load accumulator and index X
 ***************************************************************/
#define OAL 												\
A = X = (OEUInt8)((A|0xee)&tmp);							\
SET_NZ(A)

/***************************************************************
 * RLA  rotate left and logical and accumulator
 *  new C <- [7][6][5][4][3][2][1][0] <- C
 ***************************************************************/
#define RLA 												\
tmp = (tmp << 1) | (P & F_C);								\
P = (P & ~F_C) | ((tmp >> 8) & F_C);						\
tmp = (OEUInt8)tmp;											\
A &= tmp;													\
SET_NZ(A)

/***************************************************************
 * RRA  rotate right and add with carry
 *  C -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
#define RRA 												\
tmp |= (P & F_C) << 8;										\
P = (P & ~F_C) | (tmp & F_C);								\
tmp = (OEUInt8)(tmp >> 1);									\
ADC

/***************************************************************
 * SAX  logical and accumulator with index X and store
 ***************************************************************/
#define SAX 												\
tmp = A & X

/***************************************************************
 *  SLO shift left and logical or
 ***************************************************************/
#define SLO 												\
P = (P & ~F_C) | ((tmp >> 7) & F_C);						\
tmp = (OEUInt8)(tmp << 1);									\
A |= tmp;													\
SET_NZ(A)

/***************************************************************
 *  SRE logical shift right and logical exclusive or
 *  0 -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
#define SRE 												\
P = (P & ~F_C) | (tmp & F_C);								\
tmp = (OEUInt8)tmp >> 1;									\
A ^= tmp;													\
SET_NZ(A)

/***************************************************************
 * SAH  store accumulator and index X and high + 1
 * result = accumulator and index X and memory [PC+1] + 1
 ***************************************************************/
#define SAH tmp = A & X & (EAH+1)

/***************************************************************
 * SSH  store stack high
 * logical and accumulator with index X, transfer result to S
 * logical and result with memory [PC+1] + 1
 ***************************************************************/
#define SSH 												\
S = A & X;													\
tmp = S & (EAH+1)
#if 0
#define SSH													\
tmp = S = A & X;											\
tmp &= (OEUInt8)(memoryBus->read((PCW + 1) & 0xffff) + 1)
#endif

/***************************************************************
 * SXH  store index X high
 * logical and index X with memory[PC+1] and store the result
 ***************************************************************/
#define SXH tmp = X & (EAH+1)

/***************************************************************
 * SYH  store index Y and (high + 1)
 * logical and index Y with memory[PC+1] + 1 and store the result
 ***************************************************************/
#define SYH tmp = Y & (EAH+1)

/***************************************************************
 *  TOP triple no operation
 ***************************************************************/
//#define TOP 												\
//PCW+=2

/***************************************************************
 *  KIL Illegal opcode
 * processor halted: no hardware interrupt will help,
 * only reset
 ***************************************************************/
#define KIL 												\
PCW--
