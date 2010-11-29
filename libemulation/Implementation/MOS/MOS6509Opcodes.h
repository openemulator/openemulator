
/**
 * libemulator
 * MOS6509Opcodes
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS6509 opcodes.
 */

/*****************************************************************************
 *
 *   tbl6509.c
 *   6509 opcode functions and function pointer table
 *
 *   Copyright Peter Trauner, all rights reserved.
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
 *   - Opcode information based on an Intel 386 '6510.asm' core
 *     written by R.F. van Ee (1993)
 *   - Cycle counts are guesswork :-)
 *
 *****************************************************************************/

#include "MOS6502Operations.h"
#include "MOS6502IllegalOperations.h"
#include "MOS6509Operations.h"

#define MOS6509_OP(nn) case 0x##nn: MOS6509_OP##nn; break

#define MOS6509_OP00 {                  BRK;                 } /* 7 BRK */
#define MOS6509_OP20 {                  JSR;                 } /* 6 JSR */
#define MOS6509_OP40 {                  RTI;                 } /* 6 RTI */
#define MOS6509_OP60 {                  RTS;                 } /* 6 RTS */
#define MOS6509_OP80 { int tmp; RD_IMM; NOP;                 } /* 2 NOP IMM */
#define MOS6509_OPa0 { int tmp; RD_IMM; LDY;                 } /* 2 LDY IMM */
#define MOS6509_OPc0 { int tmp; RD_IMM; CPY;                 } /* 2 CPY IMM */
#define MOS6509_OPe0 { int tmp; RD_IMM; CPX;                 } /* 2 CPX IMM */

#define MOS6509_OP10 { int tmp; BPL;                         } /* 2-4 BPL REL */
#define MOS6509_OP30 { int tmp; BMI;                         } /* 2-4 BMI REL */
#define MOS6509_OP50 { int tmp; BVC;                         } /* 2-4 BVC REL */
#define MOS6509_OP70 { int tmp; BVS;                         } /* 2-4 BVS REL */
#define MOS6509_OP90 { int tmp; BCC;                         } /* 2-4 BCC REL */
#define MOS6509_OPb0 { int tmp; BCS;                         } /* 2-4 BCS REL */
#define MOS6509_OPd0 { int tmp; BNE;                         } /* 2-4 BNE REL */
#define MOS6509_OPf0 { int tmp; BEQ;                         } /* 2-4 BEQ REL */

#define MOS6509_OP01 { int tmp; RD_IDX; ORA;                 } /* 6 ORA IDX */
#define MOS6509_OP21 { int tmp; RD_IDX; AND;                 } /* 6 AND IDX */
#define MOS6509_OP41 { int tmp; RD_IDX; EOR;                 } /* 6 EOR IDX */
#define MOS6509_OP61 { int tmp; RD_IDX; ADC;                 } /* 6 ADC IDX */
#define MOS6509_OP81 { int tmp; STA; WR_IDX;                 } /* 6 STA IDX */
#define MOS6509_OPa1 { int tmp; RD_IDX; LDA;                 } /* 6 LDA IDX */
#define MOS6509_OPc1 { int tmp; RD_IDX; CMP;                 } /* 6 CMP IDX */
#define MOS6509_OPe1 { int tmp; RD_IDX; SBC;                 } /* 6 SBC IDX */

#define MOS6509_OP11 { int tmp; RD_IDY_P; ORA;               } /* 5 ORA IDY page penalty */
#define MOS6509_OP31 { int tmp; RD_IDY_P; AND;               } /* 5 AND IDY page penalty */
#define MOS6509_OP51 { int tmp; RD_IDY_P; EOR;               } /* 5 EOR IDY page penalty */
#define MOS6509_OP71 { int tmp; RD_IDY_P; ADC;               } /* 5 ADC IDY page penalty */
#define MOS6509_OP91 { int tmp; STA; WR_IDY_6509;              } /* 6 STA IDY */
#define MOS6509_OPb1 { int tmp; RD_IDY_6509; LDA;               } /* 5 LDA IDY page penalty */
#define MOS6509_OPd1 { int tmp; RD_IDY_P; CMP;               } /* 5 CMP IDY page penalty */
#define MOS6509_OPf1 { int tmp; RD_IDY_P; SBC;               } /* 5 SBC IDY page penalty */

#define MOS6509_OP02 {                  KIL;                 } /* 1 KIL */
#define MOS6509_OP22 {                  KIL;                 } /* 1 KIL */
#define MOS6509_OP42 {                  KIL;                 } /* 1 KIL */
#define MOS6509_OP62 {                  KIL;                 } /* 1 KIL */
#define MOS6509_OP82 { int tmp; RD_IMM; NOP;                 } /* 2 NOP IMM */
#define MOS6509_OPa2 { int tmp; RD_IMM; LDX;                 } /* 2 LDX IMM */
#define MOS6509_OPc2 { int tmp; RD_IMM; NOP;                 } /* 2 NOP IMM */
#define MOS6509_OPe2 { int tmp; RD_IMM; NOP;                 } /* 2 NOP IMM */

#define MOS6509_OP12 { KIL;                                  } /* 1 KIL */
#define MOS6509_OP32 { KIL;                                  } /* 1 KIL */
#define MOS6509_OP52 { KIL;                                  } /* 1 KIL */
#define MOS6509_OP72 { KIL;                                  } /* 1 KIL */
#define MOS6509_OP92 { KIL;                                  } /* 1 KIL */
#define MOS6509_OPb2 { KIL;                                  } /* 1 KIL */
#define MOS6509_OPd2 { KIL;                                  } /* 1 KIL */
#define MOS6509_OPf2 { KIL;                                  } /* 1 KIL */

#define MOS6509_OP03 { int tmp; RD_IDX; WB_EA; SLO; WB_EA;   } /* 7 SLO IDX */
#define MOS6509_OP23 { int tmp; RD_IDX; WB_EA; RLA; WB_EA;   } /* 7 RLA IDX */
#define MOS6509_OP43 { int tmp; RD_IDX; WB_EA; SRE; WB_EA;   } /* 7 SRE IDX */
#define MOS6509_OP63 { int tmp; RD_IDX; WB_EA; RRA; WB_EA;   } /* 7 RRA IDX */
#define MOS6509_OP83 { int tmp;                SAX; WR_IDX;  } /* 6 SAX IDX */
#define MOS6509_OPa3 { int tmp; RD_IDX; LAX;                 } /* 6 LAX IDX */
#define MOS6509_OPc3 { int tmp; RD_IDX; WB_EA; DCP; WB_EA;   } /* 7 DCP IDX */
#define MOS6509_OPe3 { int tmp; RD_IDX; WB_EA; ISB; WB_EA;   } /* 7 ISB IDX */

#define MOS6509_OP13 { int tmp; RD_IDY_NP; WB_EA; SLO; WB_EA; } /* 7 SLO IDY */
#define MOS6509_OP33 { int tmp; RD_IDY_NP; WB_EA; RLA; WB_EA; } /* 7 RLA IDY */
#define MOS6509_OP53 { int tmp; RD_IDY_NP; WB_EA; SRE; WB_EA; } /* 7 SRE IDY */
#define MOS6509_OP73 { int tmp; RD_IDY_NP; WB_EA; RRA; WB_EA; } /* 7 RRA IDY */
#define MOS6509_OP93 { int tmp; EA_IDY_NP; SAH; WB_EA;        } /* 5 SAH IDY */
#define MOS6509_OPb3 { int tmp; RD_IDY_P; LAX;                } /* 5 LAX IDY page penalty */
#define MOS6509_OPd3 { int tmp; RD_IDY_NP; WB_EA; DCP; WB_EA; } /* 7 DCP IDY */
#define MOS6509_OPf3 { int tmp; RD_IDY_NP; WB_EA; ISB; WB_EA; } /* 7 ISB IDY */

#define MOS6509_OP04 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define MOS6509_OP24 { int tmp; RD_ZPG; BIT;                  } /* 3 BIT ZPG */
#define MOS6509_OP44 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define MOS6509_OP64 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define MOS6509_OP84 { int tmp; STY; WR_ZPG;                  } /* 3 STY ZPG */
#define MOS6509_OPa4 { int tmp; RD_ZPG; LDY;                  } /* 3 LDY ZPG */
#define MOS6509_OPc4 { int tmp; RD_ZPG; CPY;                  } /* 3 CPY ZPG */
#define MOS6509_OPe4 { int tmp; RD_ZPG; CPX;                  } /* 3 CPX ZPG */

#define MOS6509_OP14 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define MOS6509_OP34 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define MOS6509_OP54 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define MOS6509_OP74 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define MOS6509_OP94 { int tmp; STY; WR_ZPX;                  } /* 4 STY ZPX */
#define MOS6509_OPb4 { int tmp; RD_ZPX; LDY;                  } /* 4 LDY ZPX */
#define MOS6509_OPd4 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define MOS6509_OPf4 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */

#define MOS6509_OP05 { int tmp; RD_ZPG; ORA;                  } /* 3 ORA ZPG */
#define MOS6509_OP25 { int tmp; RD_ZPG; AND;                  } /* 3 AND ZPG */
#define MOS6509_OP45 { int tmp; RD_ZPG; EOR;                  } /* 3 EOR ZPG */
#define MOS6509_OP65 { int tmp; RD_ZPG; ADC;                  } /* 3 ADC ZPG */
#define MOS6509_OP85 { int tmp; STA; WR_ZPG;                  } /* 3 STA ZPG */
#define MOS6509_OPa5 { int tmp; RD_ZPG; LDA;                  } /* 3 LDA ZPG */
#define MOS6509_OPc5 { int tmp; RD_ZPG; CMP;                  } /* 3 CMP ZPG */
#define MOS6509_OPe5 { int tmp; RD_ZPG; SBC;                  } /* 3 SBC ZPG */

#define MOS6509_OP15 { int tmp; RD_ZPX; ORA;                  } /* 4 ORA ZPX */
#define MOS6509_OP35 { int tmp; RD_ZPX; AND;                  } /* 4 AND ZPX */
#define MOS6509_OP55 { int tmp; RD_ZPX; EOR;                  } /* 4 EOR ZPX */
#define MOS6509_OP75 { int tmp; RD_ZPX; ADC;                  } /* 4 ADC ZPX */
#define MOS6509_OP95 { int tmp; STA; WR_ZPX;                  } /* 4 STA ZPX */
#define MOS6509_OPb5 { int tmp; RD_ZPX; LDA;                  } /* 4 LDA ZPX */
#define MOS6509_OPd5 { int tmp; RD_ZPX; CMP;                  } /* 4 CMP ZPX */
#define MOS6509_OPf5 { int tmp; RD_ZPX; SBC;                  } /* 4 SBC ZPX */

#define MOS6509_OP06 { int tmp; RD_ZPG; WB_EA; ASL; WB_EA;    } /* 5 ASL ZPG */
#define MOS6509_OP26 { int tmp; RD_ZPG; WB_EA; ROL; WB_EA;    } /* 5 ROL ZPG */
#define MOS6509_OP46 { int tmp; RD_ZPG; WB_EA; LSR; WB_EA;    } /* 5 LSR ZPG */
#define MOS6509_OP66 { int tmp; RD_ZPG; WB_EA; ROR; WB_EA;    } /* 5 ROR ZPG */
#define MOS6509_OP86 { int tmp; STX; WR_ZPG;                  } /* 3 STX ZPG */
#define MOS6509_OPa6 { int tmp; RD_ZPG; LDX;                  } /* 3 LDX ZPG */
#define MOS6509_OPc6 { int tmp; RD_ZPG; WB_EA; DEC; WB_EA;    } /* 5 DEC ZPG */
#define MOS6509_OPe6 { int tmp; RD_ZPG; WB_EA; INC; WB_EA;    } /* 5 INC ZPG */

#define MOS6509_OP16 { int tmp; RD_ZPX; WB_EA; ASL; WB_EA;    } /* 6 ASL ZPX */
#define MOS6509_OP36 { int tmp; RD_ZPX; WB_EA; ROL; WB_EA;    } /* 6 ROL ZPX */
#define MOS6509_OP56 { int tmp; RD_ZPX; WB_EA; LSR; WB_EA;    } /* 6 LSR ZPX */
#define MOS6509_OP76 { int tmp; RD_ZPX; WB_EA; ROR; WB_EA;    } /* 6 ROR ZPX */
#define MOS6509_OP96 { int tmp; STX; WR_ZPY;                  } /* 4 STX ZPY */
#define MOS6509_OPb6 { int tmp; RD_ZPY; LDX;                  } /* 4 LDX ZPY */
#define MOS6509_OPd6 { int tmp; RD_ZPX; WB_EA; DEC; WB_EA;    } /* 6 DEC ZPX */
#define MOS6509_OPf6 { int tmp; RD_ZPX; WB_EA; INC; WB_EA;    } /* 6 INC ZPX */

#define MOS6509_OP07 { int tmp; RD_ZPG; WB_EA; SLO; WB_EA;    } /* 5 SLO ZPG */
#define MOS6509_OP27 { int tmp; RD_ZPG; WB_EA; RLA; WB_EA;    } /* 5 RLA ZPG */
#define MOS6509_OP47 { int tmp; RD_ZPG; WB_EA; SRE; WB_EA;    } /* 5 SRE ZPG */
#define MOS6509_OP67 { int tmp; RD_ZPG; WB_EA; RRA; WB_EA;    } /* 5 RRA ZPG */
#define MOS6509_OP87 { int tmp; SAX; WR_ZPG;                  } /* 3 SAX ZPG */
#define MOS6509_OPa7 { int tmp; RD_ZPG; LAX;                  } /* 3 LAX ZPG */
#define MOS6509_OPc7 { int tmp; RD_ZPG; WB_EA; DCP; WB_EA;    } /* 5 DCP ZPG */
#define MOS6509_OPe7 { int tmp; RD_ZPG; WB_EA; ISB; WB_EA;    } /* 5 ISB ZPG */

#define MOS6509_OP17 { int tmp; RD_ZPX; WB_EA; SLO; WB_EA;    } /* 6 SLO ZPX */
#define MOS6509_OP37 { int tmp; RD_ZPX; WB_EA; RLA; WB_EA;    } /* 6 RLA ZPX */
#define MOS6509_OP57 { int tmp; RD_ZPX; WB_EA; SRE; WB_EA;    } /* 6 SRE ZPX */
#define MOS6509_OP77 { int tmp; RD_ZPX; WB_EA; RRA; WB_EA;    } /* 6 RRA ZPX */
#define MOS6509_OP97 { int tmp; SAX; WR_ZPY;                  } /* 4 SAX ZPY */
#define MOS6509_OPb7 { int tmp; RD_ZPY; LAX;                  } /* 4 LAX ZPY */
#define MOS6509_OPd7 { int tmp; RD_ZPX; WB_EA; DCP; WB_EA;    } /* 6 DCP ZPX */
#define MOS6509_OPf7 { int tmp; RD_ZPX; WB_EA; ISB; WB_EA;    } /* 6 ISB ZPX */

#define MOS6509_OP08 { RD_DUM; PHP;                           } /* 3 PHP */
#define MOS6509_OP28 { RD_DUM; PLP;                           } /* 4 PLP */
#define MOS6509_OP48 { RD_DUM; PHA;                           } /* 3 PHA */
#define MOS6509_OP68 { RD_DUM; PLA;                           } /* 4 PLA */
#define MOS6509_OP88 { RD_DUM; DEY;                           } /* 2 DEY */
#define MOS6509_OPa8 { RD_DUM; TAY;                           } /* 2 TAY */
#define MOS6509_OPc8 { RD_DUM; INY;                           } /* 2 INY */
#define MOS6509_OPe8 { RD_DUM; INX;                           } /* 2 INX */

#define MOS6509_OP18 { RD_DUM; CLC;                           } /* 2 CLC */
#define MOS6509_OP38 { RD_DUM; SEC;                           } /* 2 SEC */
#define MOS6509_OP58 { RD_DUM; CLI;                           } /* 2 CLI */
#define MOS6509_OP78 { RD_DUM; SEI;                           } /* 2 SEI */
#define MOS6509_OP98 { RD_DUM; TYA;                           } /* 2 TYA */
#define MOS6509_OPb8 { RD_DUM; CLV;                           } /* 2 CLV */
#define MOS6509_OPd8 { RD_DUM; CLD;                           } /* 2 CLD */
#define MOS6509_OPf8 { RD_DUM; SED;                           } /* 2 SED */

#define MOS6509_OP09 { int tmp; RD_IMM; ORA;                  } /* 2 ORA IMM */
#define MOS6509_OP29 { int tmp; RD_IMM; AND;                  } /* 2 AND IMM */
#define MOS6509_OP49 { int tmp; RD_IMM; EOR;                  } /* 2 EOR IMM */
#define MOS6509_OP69 { int tmp; RD_IMM; ADC;                  } /* 2 ADC IMM */
#define MOS6509_OP89 { int tmp; RD_IMM; NOP;                  } /* 2 NOP IMM */
#define MOS6509_OPa9 { int tmp; RD_IMM; LDA;                  } /* 2 LDA IMM */
#define MOS6509_OPc9 { int tmp; RD_IMM; CMP;                  } /* 2 CMP IMM */
#define MOS6509_OPe9 { int tmp; RD_IMM; SBC;                  } /* 2 SBC IMM */

#define MOS6509_OP19 { int tmp; RD_ABY_P; ORA;                } /* 4 ORA ABY page penalty */
#define MOS6509_OP39 { int tmp; RD_ABY_P; AND;                } /* 4 AND ABY page penalty */
#define MOS6509_OP59 { int tmp; RD_ABY_P; EOR;                } /* 4 EOR ABY page penalty */
#define MOS6509_OP79 { int tmp; RD_ABY_P; ADC;                } /* 4 ADC ABY page penalty */
#define MOS6509_OP99 { int tmp; STA; WR_ABY_NP;               } /* 5 STA ABY */
#define MOS6509_OPb9 { int tmp; RD_ABY_P; LDA;                } /* 4 LDA ABY page penalty */
#define MOS6509_OPd9 { int tmp; RD_ABY_P; CMP;                } /* 4 CMP ABY page penalty */
#define MOS6509_OPf9 { int tmp; RD_ABY_P; SBC;                } /* 4 SBC ABY page penalty */

#define MOS6509_OP0a { int tmp; RD_DUM; RD_ACC; ASL; WB_ACC;  } /* 2 ASL A */
#define MOS6509_OP2a { int tmp; RD_DUM; RD_ACC; ROL; WB_ACC;  } /* 2 ROL A */
#define MOS6509_OP4a { int tmp; RD_DUM; RD_ACC; LSR; WB_ACC;  } /* 2 LSR A */
#define MOS6509_OP6a { int tmp; RD_DUM; RD_ACC; ROR; WB_ACC;  } /* 2 ROR A */
#define MOS6509_OP8a { RD_DUM; TXA;                           } /* 2 TXA */
#define MOS6509_OPaa { RD_DUM; TAX;                           } /* 2 TAX */
#define MOS6509_OPca { RD_DUM; DEX;                           } /* 2 DEX */
#define MOS6509_OPea { RD_DUM; NOP;                           } /* 2 NOP */

#define MOS6509_OP1a { RD_DUM; NOP;                           } /* 2 NOP */
#define MOS6509_OP3a { RD_DUM; NOP;                           } /* 2 NOP */
#define MOS6509_OP5a { RD_DUM; NOP;                           } /* 2 NOP */
#define MOS6509_OP7a { RD_DUM; NOP;                           } /* 2 NOP */
#define MOS6509_OP9a { RD_DUM; TXS;                           } /* 2 TXS */
#define MOS6509_OPba { RD_DUM; TSX;                           } /* 2 TSX */
#define MOS6509_OPda { RD_DUM; NOP;                           } /* 2 NOP */
#define MOS6509_OPfa { RD_DUM; NOP;                           } /* 2 NOP */

#define MOS6509_OP0b { int tmp; RD_IMM; ANC;                  } /* 2 ANC IMM */
#define MOS6509_OP2b { int tmp; RD_IMM; ANC;                  } /* 2 ANC IMM */
#define MOS6509_OP4b { int tmp; RD_IMM; ASR; WB_ACC;          } /* 2 ASR IMM */
#define MOS6509_OP6b { int tmp; RD_IMM; ARR; WB_ACC;          } /* 2 ARR IMM */
#define MOS6509_OP8b { int tmp; RD_IMM; AXA;                  } /* 2 AXA IMM */
#define MOS6509_OPab { int tmp; RD_IMM; OAL;                  } /* 2 OAL IMM */
#define MOS6509_OPcb { int tmp; RD_IMM; ASX;                  } /* 2 ASX IMM */
#define MOS6509_OPeb { int tmp; RD_IMM; SBC;                  } /* 2 SBC IMM */

#define MOS6509_OP1b { int tmp; RD_ABY_NP; WB_EA; SLO; WB_EA; } /* 7 SLO ABY */
#define MOS6509_OP3b { int tmp; RD_ABY_NP; WB_EA; RLA; WB_EA; } /* 7 RLA ABY */
#define MOS6509_OP5b { int tmp; RD_ABY_NP; WB_EA; SRE; WB_EA; } /* 7 SRE ABY */
#define MOS6509_OP7b { int tmp; RD_ABY_NP; WB_EA; RRA; WB_EA; } /* 7 RRA ABY */
#define MOS6509_OP9b { int tmp; EA_ABY_NP; SSH; WB_EA;        } /* 5 SSH ABY */
#define MOS6509_OPbb { int tmp; RD_ABY_P; AST;                } /* 4 AST ABY page penalty */
#define MOS6509_OPdb { int tmp; RD_ABY_NP; WB_EA; DCP; WB_EA; } /* 7 DCP ABY */
#define MOS6509_OPfb { int tmp; RD_ABY_NP; WB_EA; ISB; WB_EA; } /* 7 ISB ABY */

#define MOS6509_OP0c { int tmp; RD_ABS; NOP;                  } /* 4 NOP ABS */
#define MOS6509_OP2c { int tmp; RD_ABS; BIT;                  } /* 4 BIT ABS */
#define MOS6509_OP4c { EA_ABS; JMP;                           } /* 3 JMP ABS */
#define MOS6509_OP6c { int tmp; EA_IND; JMP;                  } /* 5 JMP IND */
#define MOS6509_OP8c { int tmp; STY; WR_ABS;                  } /* 4 STY ABS */
#define MOS6509_OPac { int tmp; RD_ABS; LDY;                  } /* 4 LDY ABS */
#define MOS6509_OPcc { int tmp; RD_ABS; CPY;                  } /* 4 CPY ABS */
#define MOS6509_OPec { int tmp; RD_ABS; CPX;                  } /* 4 CPX ABS */

#define MOS6509_OP1c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define MOS6509_OP3c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define MOS6509_OP5c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define MOS6509_OP7c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define MOS6509_OP9c { int tmp; EA_ABX_NP; SYH; WB_EA;        } /* 5 SYH ABX */
#define MOS6509_OPbc { int tmp; RD_ABX_P; LDY;                } /* 4 LDY ABX page penalty */
#define MOS6509_OPdc { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define MOS6509_OPfc { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */

#define MOS6509_OP0d { int tmp; RD_ABS; ORA;                  } /* 4 ORA ABS */
#define MOS6509_OP2d { int tmp; RD_ABS; AND;                  } /* 4 AND ABS */
#define MOS6509_OP4d { int tmp; RD_ABS; EOR;                  } /* 4 EOR ABS */
#define MOS6509_OP6d { int tmp; RD_ABS; ADC;                  } /* 4 ADC ABS */
#define MOS6509_OP8d { int tmp; STA; WR_ABS;                  } /* 4 STA ABS */
#define MOS6509_OPad { int tmp; RD_ABS; LDA;                  } /* 4 LDA ABS */
#define MOS6509_OPcd { int tmp; RD_ABS; CMP;                  } /* 4 CMP ABS */
#define MOS6509_OPed { int tmp; RD_ABS; SBC;                  } /* 4 SBC ABS */

#define MOS6509_OP1d { int tmp; RD_ABX_P; ORA;                } /* 4 ORA ABX page penalty */
#define MOS6509_OP3d { int tmp; RD_ABX_P; AND;                } /* 4 AND ABX page penalty */
#define MOS6509_OP5d { int tmp; RD_ABX_P; EOR;                } /* 4 EOR ABX page penalty */
#define MOS6509_OP7d { int tmp; RD_ABX_P; ADC;                } /* 4 ADC ABX page penalty */
#define MOS6509_OP9d { int tmp; STA; WR_ABX_NP;               } /* 5 STA ABX */
#define MOS6509_OPbd { int tmp; RD_ABX_P; LDA;                } /* 4 LDA ABX page penalty */
#define MOS6509_OPdd { int tmp; RD_ABX_P; CMP;                } /* 4 CMP ABX page penalty */
#define MOS6509_OPfd { int tmp; RD_ABX_P; SBC;                } /* 4 SBC ABX page penalty */

#define MOS6509_OP0e { int tmp; RD_ABS; WB_EA; ASL; WB_EA;    } /* 6 ASL ABS */
#define MOS6509_OP2e { int tmp; RD_ABS; WB_EA; ROL; WB_EA;    } /* 6 ROL ABS */
#define MOS6509_OP4e { int tmp; RD_ABS; WB_EA; LSR; WB_EA;    } /* 6 LSR ABS */
#define MOS6509_OP6e { int tmp; RD_ABS; WB_EA; ROR; WB_EA;    } /* 6 ROR ABS */
#define MOS6509_OP8e { int tmp; STX; WR_ABS;                  } /* 4 STX ABS */
#define MOS6509_OPae { int tmp; RD_ABS; LDX;                  } /* 4 LDX ABS */
#define MOS6509_OPce { int tmp; RD_ABS; WB_EA; DEC; WB_EA;    } /* 6 DEC ABS */
#define MOS6509_OPee { int tmp; RD_ABS; WB_EA; INC; WB_EA;    } /* 6 INC ABS */

#define MOS6509_OP1e { int tmp; RD_ABX_NP; WB_EA; ASL; WB_EA; } /* 7 ASL ABX */
#define MOS6509_OP3e { int tmp; RD_ABX_NP; WB_EA; ROL; WB_EA; } /* 7 ROL ABX */
#define MOS6509_OP5e { int tmp; RD_ABX_NP; WB_EA; LSR; WB_EA; } /* 7 LSR ABX */
#define MOS6509_OP7e { int tmp; RD_ABX_NP; WB_EA; ROR; WB_EA; } /* 7 ROR ABX */
#define MOS6509_OP9e { int tmp; EA_ABY_NP; SXH; WB_EA;        } /* 5 SXH ABY */
#define MOS6509_OPbe { int tmp; RD_ABY_P; LDX;                } /* 4 LDX ABY page penalty */
#define MOS6509_OPde { int tmp; RD_ABX_NP; WB_EA; DEC; WB_EA; } /* 7 DEC ABX */
#define MOS6509_OPfe { int tmp; RD_ABX_NP; WB_EA; INC; WB_EA; } /* 7 INC ABX */

#define MOS6509_OP0f { int tmp; RD_ABS; WB_EA; SLO; WB_EA;    } /* 6 SLO ABS */
#define MOS6509_OP2f { int tmp; RD_ABS; WB_EA; RLA; WB_EA;    } /* 6 RLA ABS */
#define MOS6509_OP4f { int tmp; RD_ABS; WB_EA; SRE; WB_EA;    } /* 6 SRE ABS */
#define MOS6509_OP6f { int tmp; RD_ABS; WB_EA; RRA; WB_EA;    } /* 6 RRA ABS */
#define MOS6509_OP8f { int tmp; SAX; WR_ABS;                  } /* 4 SAX ABS */
#define MOS6509_OPaf { int tmp; RD_ABS; LAX;                  } /* 4 LAX ABS */
#define MOS6509_OPcf { int tmp; RD_ABS; WB_EA; DCP; WB_EA;    } /* 6 DCP ABS */
#define MOS6509_OPef { int tmp; RD_ABS; WB_EA; ISB; WB_EA;    } /* 6 ISB ABS */

#define MOS6509_OP1f { int tmp; RD_ABX_NP; WB_EA; SLO; WB_EA; } /* 7 SLO ABX */
#define MOS6509_OP3f { int tmp; RD_ABX_NP; WB_EA; RLA; WB_EA; } /* 7 RLA ABX */
#define MOS6509_OP5f { int tmp; RD_ABX_NP; WB_EA; SRE; WB_EA; } /* 7 SRE ABX */
#define MOS6509_OP7f { int tmp; RD_ABX_NP; WB_EA; RRA; WB_EA; } /* 7 RRA ABX */
#define MOS6509_OP9f { int tmp; EA_ABY_NP; SAH; WB_EA;        } /* 5 SAH ABY */
#define MOS6509_OPbf { int tmp; RD_ABY_P; LAX;                } /* 4 LAX ABY page penalty */
#define MOS6509_OPdf { int tmp; RD_ABX_NP; WB_EA; DCP; WB_EA; } /* 7 DCP ABX */
#define MOS6509_OPff { int tmp; RD_ABX_NP; WB_EA; ISB; WB_EA; } /* 7 ISB ABX */
