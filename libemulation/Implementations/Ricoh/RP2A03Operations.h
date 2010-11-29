
/**
 * libemulator
 * RP2A03Operations
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements RP2A03 operations.
 */

/***************************************************************
 ***************************************************************
 *          Macros to emulate the N2A03 opcodes
 ***************************************************************
 ***************************************************************/

/***************************************************************
 *  ADC Add with carry - no decimal mode
 ***************************************************************/
#define ADC_2A03 											\
{															\
int c = (P & F_C);											\
int sum = A + tmp + c;										\
P &= ~(F_V | F_C);											\
if( ~(A^tmp) & (A^sum) & F_N )								\
P |= F_V;													\
if( sum & 0xff00 )											\
P |= F_C;													\
A = (UINT8) sum;											\
}															\
SET_NZ(A)

/***************************************************************
 *  SBC Subtract with carry - no decimal mode
 ***************************************************************/
#define SBC_2A03 											\
{															\
int c = (P & F_C) ^ F_C;									\
int sum = A - tmp - c;										\
P &= ~(F_V | F_C);											\
if( (A^tmp) & (A^sum) & F_N )								\
P |= F_V;													\
if( (sum & 0xff00) == 0 )									\
P |= F_C;													\
A = (UINT8) sum;											\
}															\
SET_NZ(A)
