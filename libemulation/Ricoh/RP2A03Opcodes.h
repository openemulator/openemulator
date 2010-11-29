
/**
 * libemulator
 * RP2A03Opcodes
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements RP2A03 opcodes.
 */

/*****************************************************************************
 *
 *   tbl2a03.c
 *   2a03 opcode functions and function pointer table
 *
 *   The 2a03 is a 6502 CPU that does not support the decimal mode
 *   of the ADC and SBC instructions, so all opcodes except ADC/SBC
 *   are simply mapped to the m6502 ones.
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
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

#include "RP2A03Operations.h"
#include "RP2A03IllegalOperations.h"

/*
 based on the nmos 6502
 
 b flag handling might be changed,
 although only nmos series b-flag handling is quite sure
 */

#define RP2A03_OP(nn) case 0x##nn: RP2A03_OP##nn; break

/*****************************************************************************
 *****************************************************************************
 *
 *   overrides for 2a03 opcodes
 *
 *****************************************************************************
 ********** insn   temp     cycles             rdmem   opc  wrmem   **********/
#define RP2A03_OP61 { int tmp; RD_IDX; ADC_2A03;					} /* 6 ADC IDX */
#define RP2A03_OPe1 { int tmp; RD_IDX; SBC_2A03;					} /* 6 SBC IDX */
#define RP2A03_OP71 { int tmp; RD_IDY_P; ADC_2A03;					} /* 5 ADC IDY page penalty */
#define RP2A03_OPf1 { int tmp; RD_IDY_P; SBC_2A03;					} /* 5 SBC IDY page penalty */
#define RP2A03_OP63 { int tmp; RD_IDX; WB_EA; RRA_2A03; WB_EA;		} /* 7 RRA IDX */
#define RP2A03_OP73 { int tmp; RD_IDY_NP; WB_EA; RRA_2A03; WB_EA;	} /* 7 RRA IDY */
#define RP2A03_OPe3 { int tmp; RD_IDX; WB_EA; ISB_2A03; WB_EA;		} /* 7 ISB IDX */
#define RP2A03_OPf3 { int tmp; RD_IDY_NP; WB_EA; ISB_2A03; WB_EA;	} /* 7 ISB IDY */
#define RP2A03_OP65 { int tmp; RD_ZPG; ADC_2A03;					} /* 3 ADC ZPG */
#define RP2A03_OPe5 { int tmp; RD_ZPG; SBC_2A03;					} /* 3 SBC ZPG */
#define RP2A03_OP75 { int tmp; RD_ZPX; ADC_2A03;					} /* 4 ADC ZPX */
#define RP2A03_OPf5 { int tmp; RD_ZPX; SBC_2A03;					} /* 4 SBC ZPX */
#define RP2A03_OP67 { int tmp; RD_ZPG; WB_EA; RRA_2A03; WB_EA;		} /* 5 RRA ZPG */
#define RP2A03_OP77 { int tmp; RD_ZPX; WB_EA; RRA_2A03; WB_EA;		} /* 6 RRA ZPX */
#define RP2A03_OPe7 { int tmp; RD_ZPG; WB_EA; ISB_2A03; WB_EA;		} /* 5 ISB ZPG */
#define RP2A03_OPf7 { int tmp; RD_ZPX; WB_EA; ISB_2A03; WB_EA;		} /* 6 ISB ZPX */
#define RP2A03_OP69 { int tmp; RD_IMM; ADC_2A03;					} /* 2 ADC IMM */
#define RP2A03_OPe9 { int tmp; RD_IMM; SBC_2A03;					} /* 2 SBC IMM */
#define RP2A03_OP79 { int tmp; RD_ABY_P; ADC_2A03;					} /* 4 ADC ABY page penalty */
#define RP2A03_OPf9 { int tmp; RD_ABY_P; SBC_2A03;					} /* 4 SBC ABY page penalty */
#define RP2A03_OP6b { int tmp; RD_IMM; ARR_2A03; WB_ACC;			} /* 2 ARR IMM */
#define RP2A03_OP7b { int tmp; RD_ABY_NP; WB_EA; RRA_2A03; WB_EA;	} /* 7 RRA ABY */
#define RP2A03_OPab { int tmp; RD_IMM; OAL_2A03;					} /* 2 OAL IMM */
#define RP2A03_OPeb { int tmp; RD_IMM; SBC_2A03;					} /* 2 SBC IMM */
#define RP2A03_OPfb { int tmp; RD_ABY_NP; WB_EA; ISB_2A03; WB_EA;	} /* 7 ISB ABY */
#define RP2A03_OP9c { int tmp; EA_ABX_NP; SYH_2A03; WB_EA;			} /* 5 SYH ABX */
#define RP2A03_OP6d { int tmp; RD_ABS; ADC_2A03;					} /* 4 ADC ABS */
#define RP2A03_OPed { int tmp; RD_ABS; SBC_2A03;					} /* 4 SBC ABS */
#define RP2A03_OP7d { int tmp; RD_ABX_P; ADC_2A03;					} /* 4 ADC ABX page penalty */
#define RP2A03_OPfd { int tmp; RD_ABX_P; SBC_2A03;					} /* 4 SBC ABX page penalty */
#define RP2A03_OP9e { int tmp; EA_ABY_NP; SXH_2A03; WB_EA;			} /* 5 SXH ABY */
#define RP2A03_OP6f { int tmp; RD_ABS; WB_EA; RRA_2A03; WB_EA;		} /* 6 RRA ABS */
#define RP2A03_OP7f { int tmp; RD_ABX_NP; WB_EA; RRA_2A03; WB_EA;	} /* 7 RRA ABX */
#define RP2A03_OPef { int tmp; RD_ABS; WB_EA; ISB_2A03; WB_EA;		} /* 6 ISB ABS */
#define RP2A03_OPff { int tmp; RD_ABX_NP; WB_EA; ISB_2A03; WB_EA;	} /* 7 ISB ABX */
