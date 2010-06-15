
/**
 * libemulator
 * RP2A03IllegalOperations
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements RP2A03 illegal operations.
 */

/***************************************************************
 *  ARR logical and, rotate right - no decimal mode
 ***************************************************************/
#define ARR_2A03											\
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
 *  ISB increment and subtract with carry
 ***************************************************************/
#define ISB_2A03											\
tmp = (UINT8)(tmp+1);										\
SBC_2A03

/***************************************************************
 * RRA  rotate right and add with carry
 *  C -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
#define RRA_2A03											\
tmp |= (P & F_C) << 8;										\
P = (P & ~F_C) | (tmp & F_C);								\
tmp = (UINT8)(tmp >> 1);									\
ADC_2A03

/***************************************************************
 *  OAL load accumulator and index X
 ***************************************************************/
#define OAL_2A03											\
A = X = (UINT8)((A|0xff)&tmp);								\
SET_NZ(A)

/***************************************************************
 * SXH  store index X high
 * logical and index X with memory[PC+1] and store the result
 *
 * This instruction writes to an odd address when crossing
 * a page boundary. The one known test case can be explained
 * with a shift of Y. More testing will be needed to determine
 * if this is correct.
 *
 ***************************************************************/
#define SXH_2A03											\
if ( Y && Y > EAL )											\
EAH |= ( Y << 1 );											\
tmp = X & (EAH+1)

/***************************************************************
 * SYH  store index Y and (high + 1)
 * logical and index Y with memory[PC+1] + 1 and store the result
 *
 * This instruction writs to an odd address when crossing a
 * a page boundary. The one known test case can be explained
 * with a shoft of X. More testing will be needed to determine
 * if this is correct.
 *
 ***************************************************************/
#define SYH_2A03											\
if ( X && X > EAL )											\
EAH |= ( X << 1 );											\
tmp = Y & (EAH+1)
