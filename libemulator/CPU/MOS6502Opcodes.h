
/**
 * libemulator
 * MOS6502
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a MOS6502 microprocessor.
 */

/*****************************************************************************
 *
 *   tbl6502.c
 *   6502 opcode functions and function pointer table
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
 *   2003-05-26  Fixed PHP, PLP, PHA, PLA cycle counts. [SJ]
 *   2004-04-30  Fixed STX (abs) cycle count. [SJ]
 *
 *****************************************************************************/

/*****************************************************************************
 *****************************************************************************
 *
 *   plain vanilla 6502 opcodes
 *
 *****************************************************************************
 * op    temp     cycles             rdmem   opc  wrmem   ********************/

#define M6502_OP(nn) case 0x##nn: M6502_OP##nn; break

#define M6502_OP00 {                  BRK;					} /* 7 BRK */
#define M6502_OP20 {                  JSR;					} /* 6 JSR */
#define M6502_OP40 {                  RTI;					} /* 6 RTI */
#define M6502_OP60 {                  RTS;					} /* 6 RTS */
#define M6502_OP80 { int tmp; RD_IMM; NOP;					} /* 2 NOP IMM */
#define M6502_OPa0 { int tmp; RD_IMM; LDY;					} /* 2 LDY IMM */
#define M6502_OPc0 { int tmp; RD_IMM; CPY;					} /* 2 CPY IMM */
#define M6502_OPe0 { int tmp; RD_IMM; CPX;					} /* 2 CPX IMM */

#define M6502_OP10 { BPL;									} /* 2-4 BPL REL */
#define M6502_OP30 { BMI;									} /* 2-4 BMI REL */
#define M6502_OP50 { BVC;									} /* 2-4 BVC REL */
#define M6502_OP70 { BVS;									} /* 2-4 BVS REL */
#define M6502_OP90 { BCC;									} /* 2-4 BCC REL */
#define M6502_OPb0 { BCS;									} /* 2-4 BCS REL */
#define M6502_OPd0 { BNE;									} /* 2-4 BNE REL */
#define M6502_OPf0 { BEQ;									} /* 2-4 BEQ REL */

#define M6502_OP01 { int tmp; RD_IDX; ORA;					} /* 6 ORA IDX */
#define M6502_OP21 { int tmp; RD_IDX; AND;					} /* 6 AND IDX */
#define M6502_OP41 { int tmp; RD_IDX; EOR;					} /* 6 EOR IDX */
#define M6502_OP61 { int tmp; RD_IDX; ADC;					} /* 6 ADC IDX */
#define M6502_OP81 { int tmp; STA; WR_IDX;					} /* 6 STA IDX */
#define M6502_OPa1 { int tmp; RD_IDX; LDA;					} /* 6 LDA IDX */
#define M6502_OPc1 { int tmp; RD_IDX; CMP;					} /* 6 CMP IDX */
#define M6502_OPe1 { int tmp; RD_IDX; SBC;					} /* 6 SBC IDX */

#define M6502_OP11 { int tmp; RD_IDY_P; ORA;				} /* 5 ORA IDY page penalty */
#define M6502_OP31 { int tmp; RD_IDY_P; AND;				} /* 5 AND IDY page penalty */
#define M6502_OP51 { int tmp; RD_IDY_P; EOR;				} /* 5 EOR IDY page penalty */
#define M6502_OP71 { int tmp; RD_IDY_P; ADC;				} /* 5 ADC IDY page penalty */
#define M6502_OP91 { int tmp; STA; WR_IDY_NP;				} /* 6 STA IDY */
#define M6502_OPb1 { int tmp; RD_IDY_P; LDA;				} /* 5 LDA IDY page penalty */
#define M6502_OPd1 { int tmp; RD_IDY_P; CMP;				} /* 5 CMP IDY page penalty */
#define M6502_OPf1 { int tmp; RD_IDY_P; SBC;				} /* 5 SBC IDY page penalty */

#define M6502_OP02 {                  KIL;					} /* 1 KIL */
#define M6502_OP22 {                  KIL;					} /* 1 KIL */
#define M6502_OP42 {                  KIL;					} /* 1 KIL */
#define M6502_OP62 {                  KIL;					} /* 1 KIL */
#define M6502_OP82 { int tmp; RD_IMM; NOP;					} /* 2 NOP IMM */
#define M6502_OPa2 { int tmp; RD_IMM; LDX;					} /* 2 LDX IMM */
#define M6502_OPc2 { int tmp; RD_IMM; NOP;					} /* 2 NOP IMM */
#define M6502_OPe2 { int tmp; RD_IMM; NOP;					} /* 2 NOP IMM */

#define M6502_OP12 { KIL;									} /* 1 KIL */
#define M6502_OP32 { KIL;									} /* 1 KIL */
#define M6502_OP52 { KIL;									} /* 1 KIL */
#define M6502_OP72 { KIL;									} /* 1 KIL */
#define M6502_OP92 { KIL;									} /* 1 KIL */
#define M6502_OPb2 { KIL;									} /* 1 KIL */
#define M6502_OPd2 { KIL;									} /* 1 KIL */
#define M6502_OPf2 { KIL;									} /* 1 KIL */

#define M6502_OP03 { int tmp; RD_IDX; WB_EA; SLO; WB_EA;	} /* 7 SLO IDX */
#define M6502_OP23 { int tmp; RD_IDX; WB_EA; RLA; WB_EA;	} /* 7 RLA IDX */
#define M6502_OP43 { int tmp; RD_IDX; WB_EA; SRE; WB_EA;	} /* 7 SRE IDX */
#define M6502_OP63 { int tmp; RD_IDX; WB_EA; RRA; WB_EA;	} /* 7 RRA IDX */
#define M6502_OP83 { int tmp;                SAX; WR_IDX;	} /* 6 SAX IDX */
#define M6502_OPa3 { int tmp; RD_IDX; LAX;					} /* 6 LAX IDX */
#define M6502_OPc3 { int tmp; RD_IDX; WB_EA; DCP; WB_EA;	} /* 7 DCP IDX */
#define M6502_OPe3 { int tmp; RD_IDX; WB_EA; ISB; WB_EA;	} /* 7 ISB IDX */

#define M6502_OP13 { int tmp; RD_IDY_NP; WB_EA; SLO; WB_EA; } /* 7 SLO IDY */
#define M6502_OP33 { int tmp; RD_IDY_NP; WB_EA; RLA; WB_EA; } /* 7 RLA IDY */
#define M6502_OP53 { int tmp; RD_IDY_NP; WB_EA; SRE; WB_EA; } /* 7 SRE IDY */
#define M6502_OP73 { int tmp; RD_IDY_NP; WB_EA; RRA; WB_EA; } /* 7 RRA IDY */
#define M6502_OP93 { int tmp; EA_IDY_NP; SAH; WB_EA;        } /* 5 SAH IDY */
#define M6502_OPb3 { int tmp; RD_IDY_P; LAX;                } /* 5 LAX IDY page penalty */
#define M6502_OPd3 { int tmp; RD_IDY_NP; WB_EA; DCP; WB_EA; } /* 7 DCP IDY */
#define M6502_OPf3 { int tmp; RD_IDY_NP; WB_EA; ISB; WB_EA; } /* 7 ISB IDY */

#define M6502_OP04 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define M6502_OP24 { int tmp; RD_ZPG; BIT;                  } /* 3 BIT ZPG */
#define M6502_OP44 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define M6502_OP64 { int tmp; RD_ZPG; NOP;                  } /* 3 NOP ZPG */
#define M6502_OP84 { int tmp; STY; WR_ZPG;                  } /* 3 STY ZPG */
#define M6502_OPa4 { int tmp; RD_ZPG; LDY;                  } /* 3 LDY ZPG */
#define M6502_OPc4 { int tmp; RD_ZPG; CPY;                  } /* 3 CPY ZPG */
#define M6502_OPe4 { int tmp; RD_ZPG; CPX;                  } /* 3 CPX ZPG */

#define M6502_OP14 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define M6502_OP34 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define M6502_OP54 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define M6502_OP74 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define M6502_OP94 { int tmp; STY; WR_ZPX;                  } /* 4 STY ZPX */
#define M6502_OPb4 { int tmp; RD_ZPX; LDY;                  } /* 4 LDY ZPX */
#define M6502_OPd4 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */
#define M6502_OPf4 { int tmp; RD_ZPX; NOP;                  } /* 4 NOP ZPX */

#define M6502_OP05 { int tmp; RD_ZPG; ORA;                  } /* 3 ORA ZPG */
#define M6502_OP25 { int tmp; RD_ZPG; AND;                  } /* 3 AND ZPG */
#define M6502_OP45 { int tmp; RD_ZPG; EOR;                  } /* 3 EOR ZPG */
#define M6502_OP65 { int tmp; RD_ZPG; ADC;                  } /* 3 ADC ZPG */
#define M6502_OP85 { int tmp; STA; WR_ZPG;                  } /* 3 STA ZPG */
#define M6502_OPa5 { int tmp; RD_ZPG; LDA;                  } /* 3 LDA ZPG */
#define M6502_OPc5 { int tmp; RD_ZPG; CMP;                  } /* 3 CMP ZPG */
#define M6502_OPe5 { int tmp; RD_ZPG; SBC;                  } /* 3 SBC ZPG */

#define M6502_OP15 { int tmp; RD_ZPX; ORA;                  } /* 4 ORA ZPX */
#define M6502_OP35 { int tmp; RD_ZPX; AND;                  } /* 4 AND ZPX */
#define M6502_OP55 { int tmp; RD_ZPX; EOR;                  } /* 4 EOR ZPX */
#define M6502_OP75 { int tmp; RD_ZPX; ADC;                  } /* 4 ADC ZPX */
#define M6502_OP95 { int tmp; STA; WR_ZPX;                  } /* 4 STA ZPX */
#define M6502_OPb5 { int tmp; RD_ZPX; LDA;                  } /* 4 LDA ZPX */
#define M6502_OPd5 { int tmp; RD_ZPX; CMP;                  } /* 4 CMP ZPX */
#define M6502_OPf5 { int tmp; RD_ZPX; SBC;                  } /* 4 SBC ZPX */

#define M6502_OP06 { int tmp; RD_ZPG; WB_EA; ASL; WB_EA;    } /* 5 ASL ZPG */
#define M6502_OP26 { int tmp; RD_ZPG; WB_EA; ROL; WB_EA;    } /* 5 ROL ZPG */
#define M6502_OP46 { int tmp; RD_ZPG; WB_EA; LSR; WB_EA;    } /* 5 LSR ZPG */
#define M6502_OP66 { int tmp; RD_ZPG; WB_EA; ROR; WB_EA;    } /* 5 ROR ZPG */
#define M6502_OP86 { int tmp; STX; WR_ZPG;                  } /* 3 STX ZPG */
#define M6502_OPa6 { int tmp; RD_ZPG; LDX;                  } /* 3 LDX ZPG */
#define M6502_OPc6 { int tmp; RD_ZPG; WB_EA; DEC; WB_EA;    } /* 5 DEC ZPG */
#define M6502_OPe6 { int tmp; RD_ZPG; WB_EA; INC; WB_EA;    } /* 5 INC ZPG */

#define M6502_OP16 { int tmp; RD_ZPX; WB_EA; ASL; WB_EA;    } /* 6 ASL ZPX */
#define M6502_OP36 { int tmp; RD_ZPX; WB_EA; ROL; WB_EA;    } /* 6 ROL ZPX */
#define M6502_OP56 { int tmp; RD_ZPX; WB_EA; LSR; WB_EA;    } /* 6 LSR ZPX */
#define M6502_OP76 { int tmp; RD_ZPX; WB_EA; ROR; WB_EA;    } /* 6 ROR ZPX */
#define M6502_OP96 { int tmp; STX; WR_ZPY;                  } /* 4 STX ZPY */
#define M6502_OPb6 { int tmp; RD_ZPY; LDX;                  } /* 4 LDX ZPY */
#define M6502_OPd6 { int tmp; RD_ZPX; WB_EA; DEC; WB_EA;    } /* 6 DEC ZPX */
#define M6502_OPf6 { int tmp; RD_ZPX; WB_EA; INC; WB_EA;    } /* 6 INC ZPX */

#define M6502_OP07 { int tmp; RD_ZPG; WB_EA; SLO; WB_EA;    } /* 5 SLO ZPG */
#define M6502_OP27 { int tmp; RD_ZPG; WB_EA; RLA; WB_EA;    } /* 5 RLA ZPG */
#define M6502_OP47 { int tmp; RD_ZPG; WB_EA; SRE; WB_EA;    } /* 5 SRE ZPG */
#define M6502_OP67 { int tmp; RD_ZPG; WB_EA; RRA; WB_EA;    } /* 5 RRA ZPG */
#define M6502_OP87 { int tmp; SAX; WR_ZPG;                  } /* 3 SAX ZPG */
#define M6502_OPa7 { int tmp; RD_ZPG; LAX;                  } /* 3 LAX ZPG */
#define M6502_OPc7 { int tmp; RD_ZPG; WB_EA; DCP; WB_EA;    } /* 5 DCP ZPG */
#define M6502_OPe7 { int tmp; RD_ZPG; WB_EA; ISB; WB_EA;    } /* 5 ISB ZPG */

#define M6502_OP17 { int tmp; RD_ZPX; WB_EA; SLO; WB_EA;    } /* 6 SLO ZPX */
#define M6502_OP37 { int tmp; RD_ZPX; WB_EA; RLA; WB_EA;    } /* 6 RLA ZPX */
#define M6502_OP57 { int tmp; RD_ZPX; WB_EA; SRE; WB_EA;    } /* 6 SRE ZPX */
#define M6502_OP77 { int tmp; RD_ZPX; WB_EA; RRA; WB_EA;    } /* 6 RRA ZPX */
#define M6502_OP97 { int tmp; SAX; WR_ZPY;                  } /* 4 SAX ZPY */
#define M6502_OPb7 { int tmp; RD_ZPY; LAX;                  } /* 4 LAX ZPY */
#define M6502_OPd7 { int tmp; RD_ZPX; WB_EA; DCP; WB_EA;    } /* 6 DCP ZPX */
#define M6502_OPf7 { int tmp; RD_ZPX; WB_EA; ISB; WB_EA;    } /* 6 ISB ZPX */

#define M6502_OP08 { RD_DUM; PHP;                           } /* 3 PHP */
#define M6502_OP28 { RD_DUM; PLP;                           } /* 4 PLP */
#define M6502_OP48 { RD_DUM; PHA;                           } /* 3 PHA */
#define M6502_OP68 { RD_DUM; PLA;                           } /* 4 PLA */
#define M6502_OP88 { RD_DUM; DEY;                           } /* 2 DEY */
#define M6502_OPa8 { RD_DUM; TAY;                           } /* 2 TAY */
#define M6502_OPc8 { RD_DUM; INY;                           } /* 2 INY */
#define M6502_OPe8 { RD_DUM; INX;                           } /* 2 INX */

#define M6502_OP18 { RD_DUM; CLC;                           } /* 2 CLC */
#define M6502_OP38 { RD_DUM; SEC;                           } /* 2 SEC */
#define M6502_OP58 { RD_DUM; CLI;                           } /* 2 CLI */
#define M6502_OP78 { RD_DUM; SEI;                           } /* 2 SEI */
#define M6502_OP98 { RD_DUM; TYA;                           } /* 2 TYA */
#define M6502_OPb8 { RD_DUM; CLV;                           } /* 2 CLV */
#define M6502_OPd8 { RD_DUM; CLD;                           } /* 2 CLD */
#define M6502_OPf8 { RD_DUM; SED;                           } /* 2 SED */

#define M6502_OP09 { int tmp; RD_IMM; ORA;                  } /* 2 ORA IMM */
#define M6502_OP29 { int tmp; RD_IMM; AND;                  } /* 2 AND IMM */
#define M6502_OP49 { int tmp; RD_IMM; EOR;                  } /* 2 EOR IMM */
#define M6502_OP69 { int tmp; RD_IMM; ADC;                  } /* 2 ADC IMM */
#define M6502_OP89 { int tmp; RD_IMM; NOP;                  } /* 2 NOP IMM */
#define M6502_OPa9 { int tmp; RD_IMM; LDA;                  } /* 2 LDA IMM */
#define M6502_OPc9 { int tmp; RD_IMM; CMP;                  } /* 2 CMP IMM */
#define M6502_OPe9 { int tmp; RD_IMM; SBC;                  } /* 2 SBC IMM */

#define M6502_OP19 { int tmp; RD_ABY_P; ORA;                } /* 4 ORA ABY page penalty */
#define M6502_OP39 { int tmp; RD_ABY_P; AND;                } /* 4 AND ABY page penalty */
#define M6502_OP59 { int tmp; RD_ABY_P; EOR;                } /* 4 EOR ABY page penalty */
#define M6502_OP79 { int tmp; RD_ABY_P; ADC;                } /* 4 ADC ABY page penalty */
#define M6502_OP99 { int tmp; STA; WR_ABY_NP;               } /* 5 STA ABY */
#define M6502_OPb9 { int tmp; RD_ABY_P; LDA;                } /* 4 LDA ABY page penalty */
#define M6502_OPd9 { int tmp; RD_ABY_P; CMP;                } /* 4 CMP ABY page penalty */
#define M6502_OPf9 { int tmp; RD_ABY_P; SBC;                } /* 4 SBC ABY page penalty */

#define M6502_OP0a { int tmp; RD_DUM; RD_ACC; ASL; WB_ACC;  } /* 2 ASL A */
#define M6502_OP2a { int tmp; RD_DUM; RD_ACC; ROL; WB_ACC;  } /* 2 ROL A */
#define M6502_OP4a { int tmp; RD_DUM; RD_ACC; LSR; WB_ACC;  } /* 2 LSR A */
#define M6502_OP6a { int tmp; RD_DUM; RD_ACC; ROR; WB_ACC;  } /* 2 ROR A */
#define M6502_OP8a { RD_DUM; TXA;                           } /* 2 TXA */
#define M6502_OPaa { RD_DUM; TAX;                           } /* 2 TAX */
#define M6502_OPca { RD_DUM; DEX;                           } /* 2 DEX */
#define M6502_OPea { RD_DUM; NOP;                           } /* 2 NOP */

#define M6502_OP1a { RD_DUM; NOP;                           } /* 2 NOP */
#define M6502_OP3a { RD_DUM; NOP;                           } /* 2 NOP */
#define M6502_OP5a { RD_DUM; NOP;                           } /* 2 NOP */
#define M6502_OP7a { RD_DUM; NOP;                           } /* 2 NOP */
#define M6502_OP9a { RD_DUM; TXS;                           } /* 2 TXS */
#define M6502_OPba { RD_DUM; TSX;                           } /* 2 TSX */
#define M6502_OPda { RD_DUM; NOP;                           } /* 2 NOP */
#define M6502_OPfa { RD_DUM; NOP;                           } /* 2 NOP */

#define M6502_OP0b { int tmp; RD_IMM; ANC;                  } /* 2 ANC IMM */
#define M6502_OP2b { int tmp; RD_IMM; ANC;                  } /* 2 ANC IMM */
#define M6502_OP4b { int tmp; RD_IMM; ASR; WB_ACC;          } /* 2 ASR IMM */
#define M6502_OP6b { int tmp; RD_IMM; ARR; WB_ACC;          } /* 2 ARR IMM */
#define M6502_OP8b { int tmp; RD_IMM; AXA;                  } /* 2 AXA IMM */
#define M6502_OPab { int tmp; RD_IMM; OAL;                  } /* 2 OAL IMM */
#define M6502_OPcb { int tmp; RD_IMM; ASX;                  } /* 2 ASX IMM */
#define M6502_OPeb { int tmp; RD_IMM; SBC;                  } /* 2 SBC IMM */

#define M6502_OP1b { int tmp; RD_ABY_NP; WB_EA; SLO; WB_EA; } /* 7 SLO ABY */
#define M6502_OP3b { int tmp; RD_ABY_NP; WB_EA; RLA; WB_EA; } /* 7 RLA ABY */
#define M6502_OP5b { int tmp; RD_ABY_NP; WB_EA; SRE; WB_EA; } /* 7 SRE ABY */
#define M6502_OP7b { int tmp; RD_ABY_NP; WB_EA; RRA; WB_EA; } /* 7 RRA ABY */
#define M6502_OP9b { int tmp; EA_ABY_NP; SSH; WB_EA;        } /* 5 SSH ABY */
#define M6502_OPbb { int tmp; RD_ABY_P; AST;                } /* 4 AST ABY page penalty */
#define M6502_OPdb { int tmp; RD_ABY_NP; WB_EA; DCP; WB_EA; } /* 7 DCP ABY */
#define M6502_OPfb { int tmp; RD_ABY_NP; WB_EA; ISB; WB_EA; } /* 7 ISB ABY */

#define M6502_OP0c { int tmp; RD_ABS; NOP;                  } /* 4 NOP ABS */
#define M6502_OP2c { int tmp; RD_ABS; BIT;                  } /* 4 BIT ABS */
#define M6502_OP4c { EA_ABS; JMP;                           } /* 3 JMP ABS */
#define M6502_OP6c { int tmp; EA_IND; JMP;                  } /* 5 JMP IND */
#define M6502_OP8c { int tmp; STY; WR_ABS;                  } /* 4 STY ABS */
#define M6502_OPac { int tmp; RD_ABS; LDY;                  } /* 4 LDY ABS */
#define M6502_OPcc { int tmp; RD_ABS; CPY;                  } /* 4 CPY ABS */
#define M6502_OPec { int tmp; RD_ABS; CPX;                  } /* 4 CPX ABS */

#define M6502_OP1c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define M6502_OP3c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define M6502_OP5c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define M6502_OP7c { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define M6502_OP9c { int tmp; EA_ABX_NP; SYH; WB_EA;        } /* 5 SYH ABX */
#define M6502_OPbc { int tmp; RD_ABX_P; LDY;                } /* 4 LDY ABX page penalty */
#define M6502_OPdc { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */
#define M6502_OPfc { int tmp; RD_ABX_P; NOP;                } /* 4 NOP ABX page penalty */

#define M6502_OP0d { int tmp; RD_ABS; ORA;                  } /* 4 ORA ABS */
#define M6502_OP2d { int tmp; RD_ABS; AND;                  } /* 4 AND ABS */
#define M6502_OP4d { int tmp; RD_ABS; EOR;                  } /* 4 EOR ABS */
#define M6502_OP6d { int tmp; RD_ABS; ADC;                  } /* 4 ADC ABS */
#define M6502_OP8d { int tmp; STA; WR_ABS;                  } /* 4 STA ABS */
#define M6502_OPad { int tmp; RD_ABS; LDA;                  } /* 4 LDA ABS */
#define M6502_OPcd { int tmp; RD_ABS; CMP;                  } /* 4 CMP ABS */
#define M6502_OPed { int tmp; RD_ABS; SBC;                  } /* 4 SBC ABS */

#define M6502_OP1d { int tmp; RD_ABX_P; ORA;                } /* 4 ORA ABX page penalty */
#define M6502_OP3d { int tmp; RD_ABX_P; AND;                } /* 4 AND ABX page penalty */
#define M6502_OP5d { int tmp; RD_ABX_P; EOR;                } /* 4 EOR ABX page penalty */
#define M6502_OP7d { int tmp; RD_ABX_P; ADC;                } /* 4 ADC ABX page penalty */
#define M6502_OP9d { int tmp; STA; WR_ABX_NP;               } /* 5 STA ABX */
#define M6502_OPbd { int tmp; RD_ABX_P; LDA;                } /* 4 LDA ABX page penalty */
#define M6502_OPdd { int tmp; RD_ABX_P; CMP;                } /* 4 CMP ABX page penalty */
#define M6502_OPfd { int tmp; RD_ABX_P; SBC;                } /* 4 SBC ABX page penalty */

#define M6502_OP0e { int tmp; RD_ABS; WB_EA; ASL; WB_EA;    } /* 6 ASL ABS */
#define M6502_OP2e { int tmp; RD_ABS; WB_EA; ROL; WB_EA;    } /* 6 ROL ABS */
#define M6502_OP4e { int tmp; RD_ABS; WB_EA; LSR; WB_EA;    } /* 6 LSR ABS */
#define M6502_OP6e { int tmp; RD_ABS; WB_EA; ROR; WB_EA;    } /* 6 ROR ABS */
#define M6502_OP8e { int tmp; STX; WR_ABS;                  } /* 4 STX ABS */
#define M6502_OPae { int tmp; RD_ABS; LDX;                  } /* 4 LDX ABS */
#define M6502_OPce { int tmp; RD_ABS; WB_EA; DEC; WB_EA;    } /* 6 DEC ABS */
#define M6502_OPee { int tmp; RD_ABS; WB_EA; INC; WB_EA;    } /* 6 INC ABS */

#define M6502_OP1e { int tmp; RD_ABX_NP; WB_EA; ASL; WB_EA; } /* 7 ASL ABX */
#define M6502_OP3e { int tmp; RD_ABX_NP; WB_EA; ROL; WB_EA; } /* 7 ROL ABX */
#define M6502_OP5e { int tmp; RD_ABX_NP; WB_EA; LSR; WB_EA; } /* 7 LSR ABX */
#define M6502_OP7e { int tmp; RD_ABX_NP; WB_EA; ROR; WB_EA; } /* 7 ROR ABX */
#define M6502_OP9e { int tmp; EA_ABY_NP; SXH; WB_EA;		} /* 5 SXH ABY */
#define M6502_OPbe { int tmp; RD_ABY_P; LDX;				} /* 4 LDX ABY page penalty */
#define M6502_OPde { int tmp; RD_ABX_NP; WB_EA; DEC; WB_EA; } /* 7 DEC ABX */
#define M6502_OPfe { int tmp; RD_ABX_NP; WB_EA; INC; WB_EA; } /* 7 INC ABX */

#define M6502_OP0f { int tmp; RD_ABS; WB_EA; SLO; WB_EA;    } /* 6 SLO ABS */
#define M6502_OP2f { int tmp; RD_ABS; WB_EA; RLA; WB_EA;    } /* 6 RLA ABS */
#define M6502_OP4f { int tmp; RD_ABS; WB_EA; SRE; WB_EA;    } /* 6 SRE ABS */
#define M6502_OP6f { int tmp; RD_ABS; WB_EA; RRA; WB_EA;    } /* 6 RRA ABS */
#define M6502_OP8f { int tmp; SAX; WR_ABS;                  } /* 4 SAX ABS */
#define M6502_OPaf { int tmp; RD_ABS; LAX;                  } /* 4 LAX ABS */
#define M6502_OPcf { int tmp; RD_ABS; WB_EA; DCP; WB_EA;    } /* 6 DCP ABS */
#define M6502_OPef { int tmp; RD_ABS; WB_EA; ISB; WB_EA;    } /* 6 ISB ABS */

#define M6502_OP1f { int tmp; RD_ABX_NP; WB_EA; SLO; WB_EA; } /* 7 SLO ABX */
#define M6502_OP3f { int tmp; RD_ABX_NP; WB_EA; RLA; WB_EA; } /* 7 RLA ABX */
#define M6502_OP5f { int tmp; RD_ABX_NP; WB_EA; SRE; WB_EA; } /* 7 SRE ABX */
#define M6502_OP7f { int tmp; RD_ABX_NP; WB_EA; RRA; WB_EA; } /* 7 RRA ABX */
#define M6502_OP9f { int tmp; EA_ABY_NP; SAH; WB_EA;        } /* 5 SAH ABY */
#define M6502_OPbf { int tmp; RD_ABY_P; LAX;                } /* 4 LAX ABY page penalty */
#define M6502_OPdf { int tmp; RD_ABX_NP; WB_EA; DCP; WB_EA; } /* 7 DCP ABX */
#define M6502_OPff { int tmp; RD_ABX_NP; WB_EA; ISB; WB_EA; } /* 7 ISB ABX */

/* and here's the array of function pointers */

