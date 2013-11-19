
/**
 * libemulator
 * W65C02S
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements WDC 65C02S opcodes
 */

/*****************************************************************************
 *
 *   tbl65c02.c
 *   65c02 opcode functions and function pointer table
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
 * Not sure about the timing of all the extra (undocumented) NOP instructions.
 * Core may need to be split up into two 65c02 core. Not all versions supported
 * the bit operation RMB/SMB/etc.
 *
 *****************************************************************************/

#include "MOS6502Operations.h"
#include "MOS6502IllegalOperations.h"
#include "W65C02SOperations.h"

#define W65C02S_OP(nn) case 0x##nn: W65C02S_OP##nn; break

/*****************************************************************************
 *****************************************************************************
 *
 *  Implementations for 65C02 opcodes
 *
 *  There are a few slight differences between Rockwell and WDC 65C02 CPUs.
 *  The absolute indexed addressing mode RMW instructions take 6 cycles on
 *  WDC 65C02 CPU but 7 cycles on a regular 6502 and a Rockwell 65C02 CPU.
 *  TODO: Implement STP and WAI for wdc65c02.
 *
 *****************************************************************************
 * op    temp     cycles             rdmem   opc  wrmem   ********************/

#define W65C02S_OP00 { BRK_C02;                                   } /* 7 BRK */
#define W65C02S_OP20 { JSR;                                       } /* 6 JSR */
#define W65C02S_OP40 { RTI;                                       } /* 6 RTI */
#define W65C02S_OP60 { RTS;                                       } /* 6 RTS */
#define W65C02S_OP80 { int tmp; BRA_C02( 1 );                     } /* 3-4 BRA REL */
#define W65C02S_OPa0 { int tmp; RD_IMM; LDY;                      } /* 2 LDY IMM */
#define W65C02S_OPc0 { int tmp; RD_IMM; CPY;                      } /* 2 CPY IMM */
#define W65C02S_OPe0 { int tmp; RD_IMM; CPX;                      } /* 2 CPX IMM */

#define W65C02S_OP10 { int tmp; BRA_C02( ! ( P & F_N ) );         } /* 2-4 BPL REL */
#define W65C02S_OP30 { int tmp; BRA_C02(   ( P & F_N ) );         } /* 2-4 BMI REL */
#define W65C02S_OP50 { int tmp; BRA_C02( ! ( P & F_V ) );         } /* 2-4 BVC REL */
#define W65C02S_OP70 { int tmp; BRA_C02(   ( P & F_V ) );         } /* 2-4 BVS REL */
#define W65C02S_OP90 { int tmp; BRA_C02( ! ( P & F_C ) );         } /* 2-4 BCC REL */
#define W65C02S_OPb0 { int tmp; BRA_C02(   ( P & F_C ) );         } /* 2-4 BCS REL */
#define W65C02S_OPd0 { int tmp; BRA_C02( ! ( P & F_Z ) );         } /* 2-4 BNE REL */
#define W65C02S_OPf0 { int tmp; BRA_C02(   ( P & F_Z ) );         } /* 2-4 BEQ REL */

#define W65C02S_OP01 { int tmp; RD_IDX; ORA;                      } /* 6 ORA IDX */
#define W65C02S_OP21 { int tmp; RD_IDX; AND;                      } /* 6 AND IDX */
#define W65C02S_OP41 { int tmp; RD_IDX; EOR;                      } /* 6 EOR IDX */
#define W65C02S_OP61 { int tmp; RD_IDX; ADC_C02;                  } /* 6/7 ADC IDX */
#define W65C02S_OP81 { int tmp; STA; WR_IDX;                      } /* 6 STA IDX */
#define W65C02S_OPa1 { int tmp; RD_IDX; LDA;                      } /* 6 LDA IDX */
#define W65C02S_OPc1 { int tmp; RD_IDX; CMP;                      } /* 6 CMP IDX */
#define W65C02S_OPe1 { int tmp; RD_IDX; SBC_C02;                  } /* 6/7 SBC IDX */

#define W65C02S_OP11 { int tmp; RD_IDY_C02_P; ORA;                } /* 5 ORA IDY page penalty */
#define W65C02S_OP31 { int tmp; RD_IDY_C02_P; AND;                } /* 5 AND IDY page penalty */
#define W65C02S_OP51 { int tmp; RD_IDY_C02_P; EOR;                } /* 5 EOR IDY page penalty */
#define W65C02S_OP71 { int tmp; RD_IDY_C02_P; ADC_C02;            } /* 5/6 ADC IDY page penalty */
#define W65C02S_OP91 { int tmp; STA; WR_IDY_C02_NP;               } /* 6 STA IDY */
#define W65C02S_OPb1 { int tmp; RD_IDY_C02_P; LDA;                } /* 5 LDA IDY page penalty */
#define W65C02S_OPd1 { int tmp; RD_IDY_C02_P; CMP;                } /* 5 CMP IDY page penalty */
#define W65C02S_OPf1 { int tmp; RD_IDY_C02_P; SBC_C02;            } /* 5/6 SBC IDY page penalty */

#define W65C02S_OP02 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OP22 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OP42 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OP62 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OP82 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OPa2 { int tmp; RD_IMM; LDX;                      } /* 2 LDX IMM */
#define W65C02S_OPc2 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */
#define W65C02S_OPe2 { int tmp; RD_IMM; NOP;                      } /* 2 NOP not sure for rockwell */

#define W65C02S_OP12 { int tmp; RD_ZPI; ORA;                      } /* 5 ORA ZPI */
#define W65C02S_OP32 { int tmp; RD_ZPI; AND;                      } /* 5 AND ZPI */
#define W65C02S_OP52 { int tmp; RD_ZPI; EOR;                      } /* 5 EOR ZPI */
#define W65C02S_OP72 { int tmp; RD_ZPI; ADC_C02;                  } /* 5/6 ADC ZPI */
#define W65C02S_OP92 { int tmp; STA; WR_ZPI;                      } /* 5 STA ZPI */
#define W65C02S_OPb2 { int tmp; RD_ZPI; LDA;                      } /* 5 LDA ZPI */
#define W65C02S_OPd2 { int tmp; RD_ZPI; CMP;                      } /* 5 CMP ZPI */
#define W65C02S_OPf2 { int tmp; RD_ZPI; SBC_C02;                  } /* 5/6 SBC ZPI */

#define W65C02S_OP03 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP23 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP43 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP63 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP83 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPa3 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPc3 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPe3 { NOP;                                       } /* 1 NOP not sure for rockwell */

#define W65C02S_OP13 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP33 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP53 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP73 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP93 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPb3 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPd3 { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPf3 { NOP;                                       } /* 1 NOP not sure for rockwell */

#define W65C02S_OP04 { int tmp; RD_ZPG; RD_EA; TSB; WB_EA;        } /* 5 TSB ZPG */
#define W65C02S_OP24 { int tmp; RD_ZPG; BIT;                      } /* 3 BIT ZPG */
#define W65C02S_OP44 { int tmp; RD_ZPG; NOP;                      } /* 3 NOP not sure for rockwell */
#define W65C02S_OP64 { int tmp; STZ; WR_ZPG;                      } /* 3 STZ ZPG */
#define W65C02S_OP84 { int tmp; STY; WR_ZPG;                      } /* 3 STY ZPG */
#define W65C02S_OPa4 { int tmp; RD_ZPG; LDY;                      } /* 3 LDY ZPG */
#define W65C02S_OPc4 { int tmp; RD_ZPG; CPY;                      } /* 3 CPY ZPG */
#define W65C02S_OPe4 { int tmp; RD_ZPG; CPX;                      } /* 3 CPX ZPG */

#define W65C02S_OP14 { int tmp; RD_ZPG; RD_EA; TRB; WB_EA;        } /* 5 TRB ZPG */
#define W65C02S_OP34 { int tmp; RD_ZPX; BIT;                      } /* 4 BIT ZPX */
#define W65C02S_OP54 { int tmp; RD_ZPX; NOP;                      } /* 4 NOP not sure for rockwell */
#define W65C02S_OP74 { int tmp; STZ; WR_ZPX;                      } /* 4 STZ ZPX */
#define W65C02S_OP94 { int tmp; STY; WR_ZPX;                      } /* 4 STY ZPX */
#define W65C02S_OPb4 { int tmp; RD_ZPX; LDY;                      } /* 4 LDY ZPX */
#define W65C02S_OPd4 { int tmp; RD_ZPX; NOP;                      } /* 4 NOP not sure for rockwell */
#define W65C02S_OPf4 { int tmp; RD_ZPX; NOP;                      } /* 4 NOP not sure for rockwell */

#define W65C02S_OP05 { int tmp; RD_ZPG; ORA;                      } /* 3 ORA ZPG */
#define W65C02S_OP25 { int tmp; RD_ZPG; AND;                      } /* 3 AND ZPG */
#define W65C02S_OP45 { int tmp; RD_ZPG; EOR;                      } /* 3 EOR ZPG */
#define W65C02S_OP65 { int tmp; RD_ZPG; ADC_C02;                  } /* 3/4 ADC ZPG */
#define W65C02S_OP85 { int tmp; STA; WR_ZPG;                      } /* 3 STA ZPG */
#define W65C02S_OPa5 { int tmp; RD_ZPG; LDA;                      } /* 3 LDA ZPG */
#define W65C02S_OPc5 { int tmp; RD_ZPG; CMP;                      } /* 3 CMP ZPG */
#define W65C02S_OPe5 { int tmp; RD_ZPG; SBC_C02;                  } /* 3/4 SBC ZPG */

#define W65C02S_OP15 { int tmp; RD_ZPX; ORA;                      } /* 4 ORA ZPX */
#define W65C02S_OP35 { int tmp; RD_ZPX; AND;                      } /* 4 AND ZPX */
#define W65C02S_OP55 { int tmp; RD_ZPX; EOR;                      } /* 4 EOR ZPX */
#define W65C02S_OP75 { int tmp; RD_ZPX; ADC_C02;                  } /* 4/5 ADC ZPX */
#define W65C02S_OP95 { int tmp; STA; WR_ZPX;                      } /* 4 STA ZPX */
#define W65C02S_OPb5 { int tmp; RD_ZPX; LDA;                      } /* 4 LDA ZPX */
#define W65C02S_OPd5 { int tmp; RD_ZPX; CMP;                      } /* 4 CMP ZPX */
#define W65C02S_OPf5 { int tmp; RD_ZPX; SBC_C02;                  } /* 4/5 SBC ZPX */

#define W65C02S_OP06 { int tmp; RD_ZPG, RD_EA; ASL; WB_EA;        } /* 5 ASL ZPG */
#define W65C02S_OP26 { int tmp; RD_ZPG; RD_EA; ROL; WB_EA;        } /* 5 ROL ZPG */
#define W65C02S_OP46 { int tmp; RD_ZPG; RD_EA; LSR; WB_EA;        } /* 5 LSR ZPG */
#define W65C02S_OP66 { int tmp; RD_ZPG; RD_EA; ROR; WB_EA;        } /* 5 ROR ZPG */
#define W65C02S_OP86 { int tmp; STX; WR_ZPG;                      } /* 3 STX ZPG */
#define W65C02S_OPa6 { int tmp; RD_ZPG; LDX;                      } /* 3 LDX ZPG */
#define W65C02S_OPc6 { int tmp; RD_ZPG; RD_EA; DEC; WB_EA;        } /* 5 DEC ZPG */
#define W65C02S_OPe6 { int tmp; RD_ZPG; RD_EA; INC; WB_EA;        } /* 5 INC ZPG */

#define W65C02S_OP16 { int tmp; RD_ZPX; RD_EA; ASL; WB_EA;        } /* 6 ASL ZPX */
#define W65C02S_OP36 { int tmp; RD_ZPX; RD_EA; ROL; WB_EA;        } /* 6 ROL ZPX */
#define W65C02S_OP56 { int tmp; RD_ZPX; RD_EA; LSR; WB_EA;        } /* 6 LSR ZPX */
#define W65C02S_OP76 { int tmp; RD_ZPX; RD_EA; ROR; WB_EA;        } /* 6 ROR ZPX */
#define W65C02S_OP96 { int tmp; STX; WR_ZPY;                      } /* 4 STX ZPY */
#define W65C02S_OPb6 { int tmp; RD_ZPY; LDX;                      } /* 4 LDX ZPY */
#define W65C02S_OPd6 { int tmp; RD_ZPX; RD_EA; DEC; WB_EA;        } /* 6 DEC ZPX */
#define W65C02S_OPf6 { int tmp; RD_ZPX; RD_EA; INC; WB_EA;        } /* 6 INC ZPX */

#define W65C02S_OP07 { int tmp; RD_ZPG; RD_EA; RMB(0);WB_EA;      } /* 5 RMB0 ZPG */
#define W65C02S_OP27 { int tmp; RD_ZPG; RD_EA; RMB(2);WB_EA;      } /* 5 RMB2 ZPG */
#define W65C02S_OP47 { int tmp; RD_ZPG; RD_EA; RMB(4);WB_EA;      } /* 5 RMB4 ZPG */
#define W65C02S_OP67 { int tmp; RD_ZPG; RD_EA; RMB(6);WB_EA;      } /* 5 RMB6 ZPG */
#define W65C02S_OP87 { int tmp; RD_ZPG; RD_EA; SMB(0);WB_EA;      } /* 5 SMB0 ZPG */
#define W65C02S_OPa7 { int tmp; RD_ZPG; RD_EA; SMB(2);WB_EA;      } /* 5 SMB2 ZPG */
#define W65C02S_OPc7 { int tmp; RD_ZPG; RD_EA; SMB(4);WB_EA;      } /* 5 SMB4 ZPG */
#define W65C02S_OPe7 { int tmp; RD_ZPG; RD_EA; SMB(6);WB_EA;      } /* 5 SMB6 ZPG */

#define W65C02S_OP17 { int tmp; RD_ZPG; RD_EA; RMB(1);WB_EA;      } /* 5 RMB1 ZPG */
#define W65C02S_OP37 { int tmp; RD_ZPG; RD_EA; RMB(3);WB_EA;      } /* 5 RMB3 ZPG */
#define W65C02S_OP57 { int tmp; RD_ZPG; RD_EA; RMB(5);WB_EA;      } /* 5 RMB5 ZPG */
#define W65C02S_OP77 { int tmp; RD_ZPG; RD_EA; RMB(7);WB_EA;      } /* 5 RMB7 ZPG */
#define W65C02S_OP97 { int tmp; RD_ZPG; RD_EA; SMB(1);WB_EA;      } /* 5 SMB1 ZPG */
#define W65C02S_OPb7 { int tmp; RD_ZPG; RD_EA; SMB(3);WB_EA;      } /* 5 SMB3 ZPG */
#define W65C02S_OPd7 { int tmp; RD_ZPG; RD_EA; SMB(5);WB_EA;      } /* 5 SMB5 ZPG */
#define W65C02S_OPf7 { int tmp; RD_ZPG; RD_EA; SMB(7);WB_EA;      } /* 5 SMB7 ZPG */

#define W65C02S_OP08 { RD_DUM; PHP;                               } /* 3 PHP */
#define W65C02S_OP28 { RD_DUM; PLP;                               } /* 4 PLP */
#define W65C02S_OP48 { RD_DUM; PHA;                               } /* 3 PHA */
#define W65C02S_OP68 { RD_DUM; PLA;                               } /* 4 PLA */
#define W65C02S_OP88 { RD_DUM; DEY;                               } /* 2 DEY */
#define W65C02S_OPa8 { RD_DUM; TAY;                               } /* 2 TAY */
#define W65C02S_OPc8 { RD_DUM; INY;                               } /* 2 INY */
#define W65C02S_OPe8 { RD_DUM; INX;                               } /* 2 INX */

#define W65C02S_OP18 { RD_DUM; CLC;                               } /* 2 CLC */
#define W65C02S_OP38 { RD_DUM; SEC;                               } /* 2 SEC */
#define W65C02S_OP58 { RD_DUM; CLI;                               } /* 2 CLI */
#define W65C02S_OP78 { RD_DUM; SEI;                               } /* 2 SEI */
#define W65C02S_OP98 { RD_DUM; TYA;                               } /* 2 TYA */
#define W65C02S_OPb8 { RD_DUM; CLV;                               } /* 2 CLV */
#define W65C02S_OPd8 { RD_DUM; CLD;                               } /* 2 CLD */
#define W65C02S_OPf8 { RD_DUM; SED;                               } /* 2 SED */

#define W65C02S_OP09 { int tmp; RD_IMM; ORA;                      } /* 2 ORA IMM */
#define W65C02S_OP29 { int tmp; RD_IMM; AND;                      } /* 2 AND IMM */
#define W65C02S_OP49 { int tmp; RD_IMM; EOR;                      } /* 2 EOR IMM */
#define W65C02S_OP69 { int tmp; RD_IMM; ADC_C02;                  } /* 2/3 ADC IMM */
#define W65C02S_OP89 { int tmp; RD_IMM; BIT_IMM_C02;              } /* 2 BIT IMM */
#define W65C02S_OPa9 { int tmp; RD_IMM; LDA;                      } /* 2 LDA IMM */
#define W65C02S_OPc9 { int tmp; RD_IMM; CMP;                      } /* 2 CMP IMM */
#define W65C02S_OPe9 { int tmp; RD_IMM; SBC_C02;                  } /* 2/3 SBC IMM */

#define W65C02S_OP19 { int tmp; RD_ABY_C02_P; ORA;                } /* 4 ORA ABY page penalty */
#define W65C02S_OP39 { int tmp; RD_ABY_C02_P; AND;                } /* 4 AND ABY page penalty */
#define W65C02S_OP59 { int tmp; RD_ABY_C02_P; EOR;                } /* 4 EOR ABY page penalty */
#define W65C02S_OP79 { int tmp; RD_ABY_C02_P; ADC_C02;            } /* 4/5 ADC ABY page penalty */
#define W65C02S_OP99 { int tmp; STA; WR_ABY_C02_NP;               } /* 5 STA ABY */
#define W65C02S_OPb9 { int tmp; RD_ABY_C02_P; LDA;                } /* 4 LDA ABY page penalty */
#define W65C02S_OPd9 { int tmp; RD_ABY_C02_P; CMP;                } /* 4 CMP ABY page penalty */
#define W65C02S_OPf9 { int tmp; RD_ABY_C02_P; SBC_C02;            } /* 4/5 SBC ABY page penalty */

#define W65C02S_OP0a { int tmp; RD_DUM; RD_ACC; ASL; WB_ACC;      } /* 2 ASL A */
#define W65C02S_OP2a { int tmp; RD_DUM; RD_ACC; ROL; WB_ACC;      } /* 2 ROL A */
#define W65C02S_OP4a { int tmp; RD_DUM; RD_ACC; LSR; WB_ACC;      } /* 2 LSR A */
#define W65C02S_OP6a { int tmp; RD_DUM; RD_ACC; ROR; WB_ACC;      } /* 2 ROR A */
#define W65C02S_OP8a { RD_DUM; TXA;                               } /* 2 TXA */
#define W65C02S_OPaa { RD_DUM; TAX;                               } /* 2 TAX */
#define W65C02S_OPca { RD_DUM; DEX;                               } /* 2 DEX */
#define W65C02S_OPea { RD_DUM; NOP;                               } /* 2 NOP */

#define W65C02S_OP1a { RD_DUM;INA;                                } /* 2 INA */
#define W65C02S_OP3a { RD_DUM;DEA;                                } /* 2 DEA */
#define W65C02S_OP5a { RD_DUM;PHY;                                } /* 3 PHY */
#define W65C02S_OP7a { RD_DUM;PLY;                                } /* 4 PLY */
#define W65C02S_OP9a { RD_DUM; TXS;                               } /* 2 TXS */
#define W65C02S_OPba { RD_DUM; TSX;                               } /* 2 TSX */
#define W65C02S_OPda { RD_DUM;PHX;                                } /* 3 PHX */
#define W65C02S_OPfa { RD_DUM;PLX;                                } /* 4 PLX */

#define W65C02S_OP0b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP2b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP4b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP6b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP8b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPab { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPcb { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPeb { NOP;                                       } /* 1 NOP not sure for rockwell */

#define W65C02S_OP1b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP3b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP5b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP7b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OP9b { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPbb { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPdb { NOP;                                       } /* 1 NOP not sure for rockwell */
#define W65C02S_OPfb { NOP;                                       } /* 1 NOP not sure for rockwell */

#define W65C02S_OP0c { int tmp; RD_ABS; RD_EA; TSB; WB_EA;        } /* 6 TSB ABS */
#define W65C02S_OP2c { int tmp; RD_ABS; BIT;                      } /* 4 BIT ABS */
#define W65C02S_OP4c { EA_ABS; JMP;                               } /* 3 JMP ABS */
#define W65C02S_OP6c { int tmp; EA_IND_C02; JMP;                  } /* 6 JMP IND */
#define W65C02S_OP8c { int tmp; STY; WR_ABS;                      } /* 4 STY ABS */
#define W65C02S_OPac { int tmp; RD_ABS; LDY;                      } /* 4 LDY ABS */
#define W65C02S_OPcc { int tmp; RD_ABS; CPY;                      } /* 4 CPY ABS */
#define W65C02S_OPec { int tmp; RD_ABS; CPX;                      } /* 4 CPX ABS */

#define W65C02S_OP1c { int tmp; RD_ABS; RD_EA; TRB; WB_EA;        } /* 6 TRB ABS */
#define W65C02S_OP3c { int tmp; RD_ABX_C02_P; BIT;                } /* 4 BIT ABX page penalty */
#define W65C02S_OP5c { int tmp; RD_ABX_C02_NP; RD_DUM; RD_DUM; RD_DUM; RD_DUM; } /* 8 NOP ABX not sure for rockwell. Page penalty not sure */
#define W65C02S_OP7c { int tmp; EA_IAX; JMP;                      } /* 6 JMP IAX page penalty */
#define W65C02S_OP9c { int tmp; STZ; WR_ABS;                      } /* 4 STZ ABS */
#define W65C02S_OPbc { int tmp; RD_ABX_C02_P; LDY;                } /* 4 LDY ABX page penalty */
#define W65C02S_OPdc { int tmp; RD_ABX_C02_NP; NOP;               } /* 4 NOP ABX not sure for rockwell. Page penalty not sure  */
#define W65C02S_OPfc { int tmp; RD_ABX_C02_NP; NOP;               } /* 4 NOP ABX not sure for rockwell. Page penalty not sure  */

#define W65C02S_OP0d { int tmp; RD_ABS; ORA;                      } /* 4 ORA ABS */
#define W65C02S_OP2d { int tmp; RD_ABS; AND;                      } /* 4 AND ABS */
#define W65C02S_OP4d { int tmp; RD_ABS; EOR;                      } /* 4 EOR ABS */
#define W65C02S_OP6d { int tmp; RD_ABS; ADC_C02;                  } /* 4/5 ADC ABS */
#define W65C02S_OP8d { int tmp; STA; WR_ABS;                      } /* 4 STA ABS */
#define W65C02S_OPad { int tmp; RD_ABS; LDA;                      } /* 4 LDA ABS */
#define W65C02S_OPcd { int tmp; RD_ABS; CMP;                      } /* 4 CMP ABS */
#define W65C02S_OPed { int tmp; RD_ABS; SBC_C02;                  } /* 4/5 SBC ABS */

#define W65C02S_OP1d { int tmp; RD_ABX_C02_P; ORA;                } /* 4 ORA ABX page penalty */
#define W65C02S_OP3d { int tmp; RD_ABX_C02_P; AND;                } /* 4 AND ABX page penalty */
#define W65C02S_OP5d { int tmp; RD_ABX_C02_P; EOR;                } /* 4 EOR ABX page penalty */
#define W65C02S_OP7d { int tmp; RD_ABX_C02_P; ADC_C02;            } /* 4/5 ADC ABX page penalty */
#define W65C02S_OP9d { int tmp; STA; WR_ABX_C02_NP;               } /* 5 STA ABX */
#define W65C02S_OPbd { int tmp; RD_ABX_C02_P; LDA;                } /* 4 LDA ABX page penalty */
#define W65C02S_OPdd { int tmp; RD_ABX_C02_P; CMP;                } /* 4 CMP ABX page penalty */
#define W65C02S_OPfd { int tmp; RD_ABX_C02_P; SBC_C02;            } /* 4/5 SBC ABX page penalty */

#define W65C02S_OP0e { int tmp; RD_ABS; RD_EA; ASL; WB_EA;        } /* 6 ASL ABS */
#define W65C02S_OP2e { int tmp; RD_ABS; RD_EA; ROL; WB_EA;        } /* 6 ROL ABS */
#define W65C02S_OP4e { int tmp; RD_ABS; RD_EA; LSR; WB_EA;        } /* 6 LSR ABS */
#define W65C02S_OP6e { int tmp; RD_ABS; RD_EA; ROR; WB_EA;        } /* 6 ROR ABS */
#define W65C02S_OP8e { int tmp; STX; WR_ABS;                      } /* 4 STX ABS */
#define W65C02S_OPae { int tmp; RD_ABS; LDX;                      } /* 4 LDX ABS */
#define W65C02S_OPce { int tmp; RD_ABS; RD_EA; DEC; WB_EA;        } /* 6 DEC ABS */
#define W65C02S_OPee { int tmp; RD_ABS; RD_EA; INC; WB_EA;        } /* 6 INC ABS */

#define W65C02S_OP1e { int tmp; RD_ABX_C02_NP; RD_EA; ASL; WB_EA; } /* 7 ASL ABX */
#define W65C02S_OP3e { int tmp; RD_ABX_C02_NP; RD_EA; ROL; WB_EA; } /* 7 ROL ABX */
#define W65C02S_OP5e { int tmp; RD_ABX_C02_NP; RD_EA; LSR; WB_EA; } /* 7 LSR ABX */
#define W65C02S_OP7e { int tmp; RD_ABX_C02_NP; RD_EA; ROR; WB_EA; } /* 7 ROR ABX */
#define W65C02S_OP9e { int tmp; STZ; WR_ABX_C02_NP;               } /* 5 STZ ABX */
#define W65C02S_OPbe { int tmp; RD_ABY_C02_P; LDX;                } /* 4 LDX ABY page penalty */
#define W65C02S_OPde { int tmp; RD_ABX_C02_NP; RD_EA; DEC; WB_EA; } /* 7 DEC ABX */
#define W65C02S_OPfe { int tmp; RD_ABX_C02_NP; RD_EA; INC; WB_EA; } /* 7 INC ABX */

#define W65C02S_OP0f { int tmp; RD_ZPG; BBR(0);                   } /* 5-7 BBR0 ZPG */
#define W65C02S_OP2f { int tmp; RD_ZPG; BBR(2);                   } /* 5-7 BBR2 ZPG */
#define W65C02S_OP4f { int tmp; RD_ZPG; BBR(4);                   } /* 5-7 BBR4 ZPG */
#define W65C02S_OP6f { int tmp; RD_ZPG; BBR(6);                   } /* 5-7 BBR6 ZPG */
#define W65C02S_OP8f { int tmp; RD_ZPG; BBS(0);                   } /* 5-7 BBS0 ZPG */
#define W65C02S_OPaf { int tmp; RD_ZPG; BBS(2);                   } /* 5-7 BBS2 ZPG */
#define W65C02S_OPcf { int tmp; RD_ZPG; BBS(4);                   } /* 5-7 BBS4 ZPG */
#define W65C02S_OPef { int tmp; RD_ZPG; BBS(6);                   } /* 5-7 BBS6 ZPG */

#define W65C02S_OP1f { int tmp; RD_ZPG; BBR(1);                   } /* 5-7 BBR1 ZPG */
#define W65C02S_OP3f { int tmp; RD_ZPG; BBR(3);                   } /* 5-7 BBR3 ZPG */
#define W65C02S_OP5f { int tmp; RD_ZPG; BBR(5);                   } /* 5-7 BBR5 ZPG */
#define W65C02S_OP7f { int tmp; RD_ZPG; BBR(7);                   } /* 5-7 BBR7 ZPG */
#define W65C02S_OP9f { int tmp; RD_ZPG; BBS(1);                   } /* 5-7 BBS1 ZPG */
#define W65C02S_OPbf { int tmp; RD_ZPG; BBS(3);                   } /* 5-7 BBS3 ZPG */
#define W65C02S_OPdf { int tmp; RD_ZPG; BBS(5);                   } /* 5-7 BBS5 ZPG */
#define W65C02S_OPff { int tmp; RD_ZPG; BBS(7);                   } /* 5-7 BBS7 ZPG */
