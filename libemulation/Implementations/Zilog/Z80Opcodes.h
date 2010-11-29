
/**
 * libemulator
 * Z80Opcodes
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Z80 opcodes.
 */

#define O2P(prefix,opcode)  INLINE void prefix##_##opcode(z80_state *z80)

/**********************************************************
 * opcodes with CB prefix
 * rotate, shift and bit operations
 **********************************************************/
#define Z80_OPcb_00 { z80->B = RLC(z80, z80->B);												} /* RLC  B           */
#define Z80_OPcb_01 { z80->C = RLC(z80, z80->C);												} /* RLC  C           */
#define Z80_OPcb_02 { z80->D = RLC(z80, z80->D);												} /* RLC  D           */
#define Z80_OPcb_03 { z80->E = RLC(z80, z80->E);												} /* RLC  E           */
#define Z80_OPcb_04 { z80->H = RLC(z80, z80->H);												} /* RLC  H           */
#define Z80_OPcb_05 { z80->L = RLC(z80, z80->L);												} /* RLC  L           */
#define Z80_OPcb_06 { WM(z80, z80->HL, RLC(z80, RM(z80, z80->HL)));							} /* RLC  (HL)        */
#define Z80_OPcb_07 { z80->A = RLC(z80, z80->A);												} /* RLC  A           */

#define Z80_OPcb_08 { z80->B = RRC(z80, z80->B);												} /* RRC  B           */
#define Z80_OPcb_09 { z80->C = RRC(z80, z80->C);												} /* RRC  C           */
#define Z80_OPcb_0a { z80->D = RRC(z80, z80->D);												} /* RRC  D           */
#define Z80_OPcb_0b { z80->E = RRC(z80, z80->E);												} /* RRC  E           */
#define Z80_OPcb_0c { z80->H = RRC(z80, z80->H);												} /* RRC  H           */
#define Z80_OPcb_0d { z80->L = RRC(z80, z80->L);												} /* RRC  L           */
#define Z80_OPcb_0e { WM(z80, z80->HL, RRC(z80, RM(z80, z80->HL)));							} /* RRC  (HL)        */
#define Z80_OPcb_0f { z80->A = RRC(z80, z80->A);												} /* RRC  A           */

#define Z80_OPcb_10 { z80->B = RL(z80, z80->B);												} /* RL   B           */
#define Z80_OPcb_11 { z80->C = RL(z80, z80->C);												} /* RL   C           */
#define Z80_OPcb_12 { z80->D = RL(z80, z80->D);												} /* RL   D           */
#define Z80_OPcb_13 { z80->E = RL(z80, z80->E);												} /* RL   E           */
#define Z80_OPcb_14 { z80->H = RL(z80, z80->H);												} /* RL   H           */
#define Z80_OPcb_15 { z80->L = RL(z80, z80->L);												} /* RL   L           */
#define Z80_OPcb_16 { WM(z80, z80->HL, RL(z80, RM(z80, z80->HL)));							} /* RL   (HL)        */
#define Z80_OPcb_17 { z80->A = RL(z80, z80->A);												} /* RL   A           */

#define Z80_OPcb_18 { z80->B = RR(z80, z80->B);												} /* RR   B           */
#define Z80_OPcb_19 { z80->C = RR(z80, z80->C);												} /* RR   C           */
#define Z80_OPcb_1a { z80->D = RR(z80, z80->D);												} /* RR   D           */
#define Z80_OPcb_1b { z80->E = RR(z80, z80->E);												} /* RR   E           */
#define Z80_OPcb_1c { z80->H = RR(z80, z80->H);												} /* RR   H           */
#define Z80_OPcb_1d { z80->L = RR(z80, z80->L);												} /* RR   L           */
#define Z80_OPcb_1e { WM(z80, z80->HL, RR(z80, RM(z80, z80->HL)));							} /* RR   (HL)        */
#define Z80_OPcb_1f { z80->A = RR(z80, z80->A);												} /* RR   A           */

#define Z80_OPcb_20 { z80->B = SLA(z80, z80->B);												} /* SLA  B           */
#define Z80_OPcb_21 { z80->C = SLA(z80, z80->C);												} /* SLA  C           */
#define Z80_OPcb_22 { z80->D = SLA(z80, z80->D);												} /* SLA  D           */
#define Z80_OPcb_23 { z80->E = SLA(z80, z80->E);												} /* SLA  E           */
#define Z80_OPcb_24 { z80->H = SLA(z80, z80->H);												} /* SLA  H           */
#define Z80_OPcb_25 { z80->L = SLA(z80, z80->L);												} /* SLA  L           */
#define Z80_OPcb_26 { WM(z80, z80->HL, SLA(z80, RM(z80, z80->HL)));							} /* SLA  (HL)        */
#define Z80_OPcb_27 { z80->A = SLA(z80, z80->A);												} /* SLA  A           */

#define Z80_OPcb_28 { z80->B = SRA(z80, z80->B);												} /* SRA  B           */
#define Z80_OPcb_29 { z80->C = SRA(z80, z80->C);												} /* SRA  C           */
#define Z80_OPcb_2a { z80->D = SRA(z80, z80->D);												} /* SRA  D           */
#define Z80_OPcb_2b { z80->E = SRA(z80, z80->E);												} /* SRA  E           */
#define Z80_OPcb_2c { z80->H = SRA(z80, z80->H);												} /* SRA  H           */
#define Z80_OPcb_2d { z80->L = SRA(z80, z80->L);												} /* SRA  L           */
#define Z80_OPcb_2e { WM(z80, z80->HL, SRA(z80, RM(z80, z80->HL)));							} /* SRA  (HL)        */
#define Z80_OPcb_2f { z80->A = SRA(z80, z80->A);												} /* SRA  A           */

#define Z80_OPcb_30 { z80->B = SLL(z80, z80->B);												} /* SLL  B           */
#define Z80_OPcb_31 { z80->C = SLL(z80, z80->C);												} /* SLL  C           */
#define Z80_OPcb_32 { z80->D = SLL(z80, z80->D);												} /* SLL  D           */
#define Z80_OPcb_33 { z80->E = SLL(z80, z80->E);												} /* SLL  E           */
#define Z80_OPcb_34 { z80->H = SLL(z80, z80->H);												} /* SLL  H           */
#define Z80_OPcb_35 { z80->L = SLL(z80, z80->L);												} /* SLL  L           */
#define Z80_OPcb_36 { WM(z80, z80->HL, SLL(z80, RM(z80, z80->HL)));							} /* SLL  (HL)        */
#define Z80_OPcb_37 { z80->A = SLL(z80, z80->A);												} /* SLL  A           */

#define Z80_OPcb_38 { z80->B = SRL(z80, z80->B);												} /* SRL  B           */
#define Z80_OPcb_39 { z80->C = SRL(z80, z80->C);												} /* SRL  C           */
#define Z80_OPcb_3a { z80->D = SRL(z80, z80->D);												} /* SRL  D           */
#define Z80_OPcb_3b { z80->E = SRL(z80, z80->E);												} /* SRL  E           */
#define Z80_OPcb_3c { z80->H = SRL(z80, z80->H);												} /* SRL  H           */
#define Z80_OPcb_3d { z80->L = SRL(z80, z80->L);												} /* SRL  L           */
#define Z80_OPcb_3e { WM(z80, z80->HL, SRL(z80, RM(z80, z80->HL)));							} /* SRL  (HL)        */
#define Z80_OPcb_3f { z80->A = SRL(z80, z80->A);												} /* SRL  A           */

#define Z80_OPcb_40 { BIT(z80, 0, z80->B);													} /* BIT  0,B         */
#define Z80_OPcb_41 { BIT(z80, 0, z80->C);													} /* BIT  0,C         */
#define Z80_OPcb_42 { BIT(z80, 0, z80->D);													} /* BIT  0,D         */
#define Z80_OPcb_43 { BIT(z80, 0, z80->E);													} /* BIT  0,E         */
#define Z80_OPcb_44 { BIT(z80, 0, z80->H);													} /* BIT  0,H         */
#define Z80_OPcb_45 { BIT(z80, 0, z80->L);													} /* BIT  0,L         */
#define Z80_OPcb_46 { BIT_HL(z80, 0, RM(z80, z80->HL));										} /* BIT  0,(HL)      */
#define Z80_OPcb_47 { BIT(z80, 0, z80->A);													} /* BIT  0,A         */

#define Z80_OPcb_48 { BIT(z80, 1, z80->B);													} /* BIT  1,B         */
#define Z80_OPcb_49 { BIT(z80, 1, z80->C);													} /* BIT  1,C         */
#define Z80_OPcb_4a { BIT(z80, 1, z80->D);													} /* BIT  1,D         */
#define Z80_OPcb_4b { BIT(z80, 1, z80->E);													} /* BIT  1,E         */
#define Z80_OPcb_4c { BIT(z80, 1, z80->H);													} /* BIT  1,H         */
#define Z80_OPcb_4d { BIT(z80, 1, z80->L);													} /* BIT  1,L         */
#define Z80_OPcb_4e { BIT_HL(z80, 1, RM(z80, z80->HL));										} /* BIT  1,(HL)      */
#define Z80_OPcb_4f { BIT(z80, 1, z80->A);													} /* BIT  1,A         */

#define Z80_OPcb_50 { BIT(z80, 2, z80->B);													} /* BIT  2,B         */
#define Z80_OPcb_51 { BIT(z80, 2, z80->C);													} /* BIT  2,C         */
#define Z80_OPcb_52 { BIT(z80, 2, z80->D);													} /* BIT  2,D         */
#define Z80_OPcb_53 { BIT(z80, 2, z80->E);													} /* BIT  2,E         */
#define Z80_OPcb_54 { BIT(z80, 2, z80->H);													} /* BIT  2,H         */
#define Z80_OPcb_55 { BIT(z80, 2, z80->L);													} /* BIT  2,L         */
#define Z80_OPcb_56 { BIT_HL(z80, 2, RM(z80, z80->HL));										} /* BIT  2,(HL)      */
#define Z80_OPcb_57 { BIT(z80, 2, z80->A);													} /* BIT  2,A         */

#define Z80_OPcb_58 { BIT(z80, 3, z80->B);													} /* BIT  3,B         */
#define Z80_OPcb_59 { BIT(z80, 3, z80->C);													} /* BIT  3,C         */
#define Z80_OPcb_5a { BIT(z80, 3, z80->D);													} /* BIT  3,D         */
#define Z80_OPcb_5b { BIT(z80, 3, z80->E);													} /* BIT  3,E         */
#define Z80_OPcb_5c { BIT(z80, 3, z80->H);													} /* BIT  3,H         */
#define Z80_OPcb_5d { BIT(z80, 3, z80->L);													} /* BIT  3,L         */
#define Z80_OPcb_5e { BIT_HL(z80, 3, RM(z80, z80->HL));										} /* BIT  3,(HL)      */
#define Z80_OPcb_5f { BIT(z80, 3, z80->A);													} /* BIT  3,A         */

#define Z80_OPcb_60 { BIT(z80, 4, z80->B);													} /* BIT  4,B         */
#define Z80_OPcb_61 { BIT(z80, 4, z80->C);													} /* BIT  4,C         */
#define Z80_OPcb_62 { BIT(z80, 4, z80->D);													} /* BIT  4,D         */
#define Z80_OPcb_63 { BIT(z80, 4, z80->E);													} /* BIT  4,E         */
#define Z80_OPcb_64 { BIT(z80, 4, z80->H);													} /* BIT  4,H         */
#define Z80_OPcb_65 { BIT(z80, 4, z80->L);													} /* BIT  4,L         */
#define Z80_OPcb_66 { BIT_HL(z80, 4, RM(z80, z80->HL));										} /* BIT  4,(HL)      */
#define Z80_OPcb_67 { BIT(z80, 4, z80->A);													} /* BIT  4,A         */

#define Z80_OPcb_68 { BIT(z80, 5, z80->B);													} /* BIT  5,B         */
#define Z80_OPcb_69 { BIT(z80, 5, z80->C);													} /* BIT  5,C         */
#define Z80_OPcb_6a { BIT(z80, 5, z80->D);													} /* BIT  5,D         */
#define Z80_OPcb_6b { BIT(z80, 5, z80->E);													} /* BIT  5,E         */
#define Z80_OPcb_6c { BIT(z80, 5, z80->H);													} /* BIT  5,H         */
#define Z80_OPcb_6d { BIT(z80, 5, z80->L);													} /* BIT  5,L         */
#define Z80_OPcb_6e { BIT_HL(z80, 5, RM(z80, z80->HL));										} /* BIT  5,(HL)      */
#define Z80_OPcb_6f { BIT(z80, 5, z80->A);													} /* BIT  5,A         */

#define Z80_OPcb_70 { BIT(z80, 6, z80->B);													} /* BIT  6,B         */
#define Z80_OPcb_71 { BIT(z80, 6, z80->C);													} /* BIT  6,C         */
#define Z80_OPcb_72 { BIT(z80, 6, z80->D);													} /* BIT  6,D         */
#define Z80_OPcb_73 { BIT(z80, 6, z80->E);													} /* BIT  6,E         */
#define Z80_OPcb_74 { BIT(z80, 6, z80->H);													} /* BIT  6,H         */
#define Z80_OPcb_75 { BIT(z80, 6, z80->L);													} /* BIT  6,L         */
#define Z80_OPcb_76 { BIT_HL(z80, 6, RM(z80, z80->HL));										} /* BIT  6,(HL)      */
#define Z80_OPcb_77 { BIT(z80, 6, z80->A);													} /* BIT  6,A         */

#define Z80_OPcb_78 { BIT(z80, 7, z80->B);													} /* BIT  7,B         */
#define Z80_OPcb_79 { BIT(z80, 7, z80->C);													} /* BIT  7,C         */
#define Z80_OPcb_7a { BIT(z80, 7, z80->D);													} /* BIT  7,D         */
#define Z80_OPcb_7b { BIT(z80, 7, z80->E);													} /* BIT  7,E         */
#define Z80_OPcb_7c { BIT(z80, 7, z80->H);													} /* BIT  7,H         */
#define Z80_OPcb_7d { BIT(z80, 7, z80->L);													} /* BIT  7,L         */
#define Z80_OPcb_7e { BIT_HL(z80, 7, RM(z80, z80->HL));										} /* BIT  7,(HL)      */
#define Z80_OPcb_7f { BIT(z80, 7, z80->A);													} /* BIT  7,A         */

#define Z80_OPcb_80 { z80->B = RES(0, z80->B);												} /* RES  0,B         */
#define Z80_OPcb_81 { z80->C = RES(0, z80->C);												} /* RES  0,C         */
#define Z80_OPcb_82 { z80->D = RES(0, z80->D);												} /* RES  0,D         */
#define Z80_OPcb_83 { z80->E = RES(0, z80->E);												} /* RES  0,E         */
#define Z80_OPcb_84 { z80->H = RES(0, z80->H);												} /* RES  0,H         */
#define Z80_OPcb_85 { z80->L = RES(0, z80->L);												} /* RES  0,L         */
#define Z80_OPcb_86 { WM(z80, z80->HL, RES(0, RM(z80, z80->HL)));								} /* RES  0,(HL)      */
#define Z80_OPcb_87 { z80->A = RES(0, z80->A);												} /* RES  0,A         */

#define Z80_OPcb_88 { z80->B = RES(1, z80->B);												} /* RES  1,B         */
#define Z80_OPcb_89 { z80->C = RES(1, z80->C);												} /* RES  1,C         */
#define Z80_OPcb_8a { z80->D = RES(1, z80->D);												} /* RES  1,D         */
#define Z80_OPcb_8b { z80->E = RES(1, z80->E);												} /* RES  1,E         */
#define Z80_OPcb_8c { z80->H = RES(1, z80->H);												} /* RES  1,H         */
#define Z80_OPcb_8d { z80->L = RES(1, z80->L);												} /* RES  1,L         */
#define Z80_OPcb_8e { WM(z80, z80->HL, RES(1, RM(z80, z80->HL)));								} /* RES  1,(HL)      */
#define Z80_OPcb_8f { z80->A = RES(1, z80->A);												} /* RES  1,A         */

#define Z80_OPcb_90 { z80->B = RES(2, z80->B);												} /* RES  2,B         */
#define Z80_OPcb_91 { z80->C = RES(2, z80->C);												} /* RES  2,C         */
#define Z80_OPcb_92 { z80->D = RES(2, z80->D);												} /* RES  2,D         */
#define Z80_OPcb_93 { z80->E = RES(2, z80->E);												} /* RES  2,E         */
#define Z80_OPcb_94 { z80->H = RES(2, z80->H);												} /* RES  2,H         */
#define Z80_OPcb_95 { z80->L = RES(2, z80->L);												} /* RES  2,L         */
#define Z80_OPcb_96 { WM(z80, z80->HL, RES(2, RM(z80, z80->HL)));								} /* RES  2,(HL)      */
#define Z80_OPcb_97 { z80->A = RES(2, z80->A);												} /* RES  2,A         */

#define Z80_OPcb_98 { z80->B = RES(3, z80->B);												} /* RES  3,B         */
#define Z80_OPcb_99 { z80->C = RES(3, z80->C);												} /* RES  3,C         */
#define Z80_OPcb_9a { z80->D = RES(3, z80->D);												} /* RES  3,D         */
#define Z80_OPcb_9b { z80->E = RES(3, z80->E);												} /* RES  3,E         */
#define Z80_OPcb_9c { z80->H = RES(3, z80->H);												} /* RES  3,H         */
#define Z80_OPcb_9d { z80->L = RES(3, z80->L);												} /* RES  3,L         */
#define Z80_OPcb_9e { WM(z80, z80->HL, RES(3, RM(z80, z80->HL)));								} /* RES  3,(HL)      */
#define Z80_OPcb_9f { z80->A = RES(3, z80->A);												} /* RES  3,A         */

#define Z80_OPcb_a0 { z80->B = RES(4,	z80->B);												} /* RES  4,B         */
#define Z80_OPcb_a1 { z80->C = RES(4,	z80->C);												} /* RES  4,C         */
#define Z80_OPcb_a2 { z80->D = RES(4,	z80->D);												} /* RES  4,D         */
#define Z80_OPcb_a3 { z80->E = RES(4,	z80->E);												} /* RES  4,E         */
#define Z80_OPcb_a4 { z80->H = RES(4,	z80->H);												} /* RES  4,H         */
#define Z80_OPcb_a5 { z80->L = RES(4,	z80->L);												} /* RES  4,L         */
#define Z80_OPcb_a6 { WM(z80, z80->HL, RES(4,	RM(z80, z80->HL)));								} /* RES  4,(HL)      */
#define Z80_OPcb_a7 { z80->A = RES(4,	z80->A);												} /* RES  4,A         */

#define Z80_OPcb_a8 { z80->B = RES(5, z80->B);												} /* RES  5,B         */
#define Z80_OPcb_a9 { z80->C = RES(5, z80->C);												} /* RES  5,C         */
#define Z80_OPcb_aa { z80->D = RES(5, z80->D);												} /* RES  5,D         */
#define Z80_OPcb_ab { z80->E = RES(5, z80->E);												} /* RES  5,E         */
#define Z80_OPcb_ac { z80->H = RES(5, z80->H);												} /* RES  5,H         */
#define Z80_OPcb_ad { z80->L = RES(5, z80->L);												} /* RES  5,L         */
#define Z80_OPcb_ae { WM(z80, z80->HL, RES(5, RM(z80, z80->HL)));								} /* RES  5,(HL)      */
#define Z80_OPcb_af { z80->A = RES(5, z80->A);												} /* RES  5,A         */

#define Z80_OPcb_b0 { z80->B = RES(6, z80->B);												} /* RES  6,B         */
#define Z80_OPcb_b1 { z80->C = RES(6, z80->C);												} /* RES  6,C         */
#define Z80_OPcb_b2 { z80->D = RES(6, z80->D);												} /* RES  6,D         */
#define Z80_OPcb_b3 { z80->E = RES(6, z80->E);												} /* RES  6,E         */
#define Z80_OPcb_b4 { z80->H = RES(6, z80->H);												} /* RES  6,H         */
#define Z80_OPcb_b5 { z80->L = RES(6, z80->L);												} /* RES  6,L         */
#define Z80_OPcb_b6 { WM(z80, z80->HL, RES(6, RM(z80, z80->HL)));								} /* RES  6,(HL)      */
#define Z80_OPcb_b7 { z80->A = RES(6, z80->A);												} /* RES  6,A         */

#define Z80_OPcb_b8 { z80->B = RES(7, z80->B);												} /* RES  7,B         */
#define Z80_OPcb_b9 { z80->C = RES(7, z80->C);												} /* RES  7,C         */
#define Z80_OPcb_ba { z80->D = RES(7, z80->D);												} /* RES  7,D         */
#define Z80_OPcb_bb { z80->E = RES(7, z80->E);												} /* RES  7,E         */
#define Z80_OPcb_bc { z80->H = RES(7, z80->H);												} /* RES  7,H         */
#define Z80_OPcb_bd { z80->L = RES(7, z80->L);												} /* RES  7,L         */
#define Z80_OPcb_be { WM(z80, z80->HL, RES(7, RM(z80, z80->HL)));								} /* RES  7,(HL)      */
#define Z80_OPcb_bf { z80->A = RES(7, z80->A);												} /* RES  7,A         */

#define Z80_OPcb_c0 { z80->B = SET(0, z80->B);												} /* SET  0,B         */
#define Z80_OPcb_c1 { z80->C = SET(0, z80->C);												} /* SET  0,C         */
#define Z80_OPcb_c2 { z80->D = SET(0, z80->D);												} /* SET  0,D         */
#define Z80_OPcb_c3 { z80->E = SET(0, z80->E);												} /* SET  0,E         */
#define Z80_OPcb_c4 { z80->H = SET(0, z80->H);												} /* SET  0,H         */
#define Z80_OPcb_c5 { z80->L = SET(0, z80->L);												} /* SET  0,L         */
#define Z80_OPcb_c6 { WM(z80, z80->HL, SET(0, RM(z80, z80->HL)));								} /* SET  0,(HL)      */
#define Z80_OPcb_c7 { z80->A = SET(0, z80->A);												} /* SET  0,A         */

#define Z80_OPcb_c8 { z80->B = SET(1, z80->B);												} /* SET  1,B         */
#define Z80_OPcb_c9 { z80->C = SET(1, z80->C);												} /* SET  1,C         */
#define Z80_OPcb_ca { z80->D = SET(1, z80->D);												} /* SET  1,D         */
#define Z80_OPcb_cb { z80->E = SET(1, z80->E);												} /* SET  1,E         */
#define Z80_OPcb_cc { z80->H = SET(1, z80->H);												} /* SET  1,H         */
#define Z80_OPcb_cd { z80->L = SET(1, z80->L);												} /* SET  1,L         */
#define Z80_OPcb_ce { WM(z80, z80->HL, SET(1, RM(z80, z80->HL)));								} /* SET  1,(HL)      */
#define Z80_OPcb_cf { z80->A = SET(1, z80->A);												} /* SET  1,A         */

#define Z80_OPcb_d0 { z80->B = SET(2, z80->B);												} /* SET  2,B         */
#define Z80_OPcb_d1 { z80->C = SET(2, z80->C);												} /* SET  2,C         */
#define Z80_OPcb_d2 { z80->D = SET(2, z80->D);												} /* SET  2,D         */
#define Z80_OPcb_d3 { z80->E = SET(2, z80->E);												} /* SET  2,E         */
#define Z80_OPcb_d4 { z80->H = SET(2, z80->H);												} /* SET  2,H         */
#define Z80_OPcb_d5 { z80->L = SET(2, z80->L);												} /* SET  2,L         */
#define Z80_OPcb_d6 { WM(z80, z80->HL, SET(2, RM(z80, z80->HL)));								} /* SET  2,(HL)      */
#define Z80_OPcb_d7 { z80->A = SET(2, z80->A);												} /* SET  2,A         */

#define Z80_OPcb_d8 { z80->B = SET(3, z80->B);												} /* SET  3,B         */
#define Z80_OPcb_d9 { z80->C = SET(3, z80->C);												} /* SET  3,C         */
#define Z80_OPcb_da { z80->D = SET(3, z80->D);												} /* SET  3,D         */
#define Z80_OPcb_db { z80->E = SET(3, z80->E);												} /* SET  3,E         */
#define Z80_OPcb_dc { z80->H = SET(3, z80->H);												} /* SET  3,H         */
#define Z80_OPcb_dd { z80->L = SET(3, z80->L);												} /* SET  3,L         */
#define Z80_OPcb_de { WM(z80, z80->HL, SET(3, RM(z80, z80->HL)));								} /* SET  3,(HL)      */
#define Z80_OPcb_df { z80->A = SET(3, z80->A);												} /* SET  3,A         */

#define Z80_OPcb_e0 { z80->B = SET(4, z80->B);												} /* SET  4,B         */
#define Z80_OPcb_e1 { z80->C = SET(4, z80->C);												} /* SET  4,C         */
#define Z80_OPcb_e2 { z80->D = SET(4, z80->D);												} /* SET  4,D         */
#define Z80_OPcb_e3 { z80->E = SET(4, z80->E);												} /* SET  4,E         */
#define Z80_OPcb_e4 { z80->H = SET(4, z80->H);												} /* SET  4,H         */
#define Z80_OPcb_e5 { z80->L = SET(4, z80->L);												} /* SET  4,L         */
#define Z80_OPcb_e6 { WM(z80, z80->HL, SET(4, RM(z80, z80->HL)));								} /* SET  4,(HL)      */
#define Z80_OPcb_e7 { z80->A = SET(4, z80->A);												} /* SET  4,A         */

#define Z80_OPcb_e8 { z80->B = SET(5, z80->B);												} /* SET  5,B         */
#define Z80_OPcb_e9 { z80->C = SET(5, z80->C);												} /* SET  5,C         */
#define Z80_OPcb_ea { z80->D = SET(5, z80->D);												} /* SET  5,D         */
#define Z80_OPcb_eb { z80->E = SET(5, z80->E);												} /* SET  5,E         */
#define Z80_OPcb_ec { z80->H = SET(5, z80->H);												} /* SET  5,H         */
#define Z80_OPcb_ed { z80->L = SET(5, z80->L);												} /* SET  5,L         */
#define Z80_OPcb_ee { WM(z80, z80->HL, SET(5, RM(z80, z80->HL)));								} /* SET  5,(HL)      */
#define Z80_OPcb_ef { z80->A = SET(5, z80->A);												} /* SET  5,A         */

#define Z80_OPcb_f0 { z80->B = SET(6, z80->B);												} /* SET  6,B         */
#define Z80_OPcb_f1 { z80->C = SET(6, z80->C);												} /* SET  6,C         */
#define Z80_OPcb_f2 { z80->D = SET(6, z80->D);												} /* SET  6,D         */
#define Z80_OPcb_f3 { z80->E = SET(6, z80->E);												} /* SET  6,E         */
#define Z80_OPcb_f4 { z80->H = SET(6, z80->H);												} /* SET  6,H         */
#define Z80_OPcb_f5 { z80->L = SET(6, z80->L);												} /* SET  6,L         */
#define Z80_OPcb_f6 { WM(z80, z80->HL, SET(6, RM(z80, z80->HL)));								} /* SET  6,(HL)      */
#define Z80_OPcb_f7 { z80->A = SET(6, z80->A);												} /* SET  6,A         */

#define Z80_OPcb_f8 { z80->B = SET(7, z80->B);												} /* SET  7,B         */
#define Z80_OPcb_f9 { z80->C = SET(7, z80->C);												} /* SET  7,C         */
#define Z80_OPcb_fa { z80->D = SET(7, z80->D);												} /* SET  7,D         */
#define Z80_OPcb_fb { z80->E = SET(7, z80->E);												} /* SET  7,E         */
#define Z80_OPcb_fc { z80->H = SET(7, z80->H);												} /* SET  7,H         */
#define Z80_OPcb_fd { z80->L = SET(7, z80->L);												} /* SET  7,L         */
#define Z80_OPcb_fe { WM(z80, z80->HL, SET(7, RM(z80, z80->HL)));								} /* SET  7,(HL)      */
#define Z80_OPcb_ff { z80->A = SET(7, z80->A);												} /* SET  7,A         */


/**********************************************************
 * opcodes with DD/FD CB prefix
 * rotate, shift and bit operations with (IX+o)
 **********************************************************/
#define Z80_OPxycb_00 { z80->B = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RLC  B=(XY+o)    */
#define Z80_OPxycb_01 { z80->C = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RLC  C=(XY+o)    */
#define Z80_OPxycb_02 { z80->D = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RLC  D=(XY+o)    */
#define Z80_OPxycb_03 { z80->E = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RLC  E=(XY+o)    */
#define Z80_OPxycb_04 { z80->H = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RLC  H=(XY+o)    */
#define Z80_OPxycb_05 { z80->L = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RLC  L=(XY+o)    */
#define Z80_OPxycb_06 { WM(z80, z80->ea, RLC(z80, RM(z80, z80->ea)));							} /* RLC  (XY+o)      */
#define Z80_OPxycb_07 { z80->A = RLC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RLC  A=(XY+o)    */

#define Z80_OPxycb_08 { z80->B = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RRC  B=(XY+o)    */
#define Z80_OPxycb_09 { z80->C = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RRC  C=(XY+o)    */
#define Z80_OPxycb_0a { z80->D = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RRC  D=(XY+o)    */
#define Z80_OPxycb_0b { z80->E = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RRC  E=(XY+o)    */
#define Z80_OPxycb_0c { z80->H = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RRC  H=(XY+o)    */
#define Z80_OPxycb_0d { z80->L = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RRC  L=(XY+o)    */
#define Z80_OPxycb_0e { WM(z80, z80->ea,RRC(z80, RM(z80, z80->ea)));							} /* RRC  (XY+o)      */
#define Z80_OPxycb_0f { z80->A = RRC(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RRC  A=(XY+o)    */

#define Z80_OPxycb_10 { z80->B = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RL   B=(XY+o)    */
#define Z80_OPxycb_11 { z80->C = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RL   C=(XY+o)    */
#define Z80_OPxycb_12 { z80->D = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RL   D=(XY+o)    */
#define Z80_OPxycb_13 { z80->E = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RL   E=(XY+o)    */
#define Z80_OPxycb_14 { z80->H = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RL   H=(XY+o)    */
#define Z80_OPxycb_15 { z80->L = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RL   L=(XY+o)    */
#define Z80_OPxycb_16 { WM(z80, z80->ea,RL(z80, RM(z80, z80->ea)));							} /* RL   (XY+o)      */
#define Z80_OPxycb_17 { z80->A = RL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RL   A=(XY+o)    */

#define Z80_OPxycb_18 { z80->B = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RR   B=(XY+o)    */
#define Z80_OPxycb_19 { z80->C = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RR   C=(XY+o)    */
#define Z80_OPxycb_1a { z80->D = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RR   D=(XY+o)    */
#define Z80_OPxycb_1b { z80->E = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RR   E=(XY+o)    */
#define Z80_OPxycb_1c { z80->H = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RR   H=(XY+o)    */
#define Z80_OPxycb_1d { z80->L = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RR   L=(XY+o)    */
#define Z80_OPxycb_1e { WM(z80, z80->ea,RR(z80, RM(z80, z80->ea)));							} /* RR   (XY+o)      */
#define Z80_OPxycb_1f { z80->A = RR(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RR   A=(XY+o)    */

#define Z80_OPxycb_20 { z80->B = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SLA  B=(XY+o)    */
#define Z80_OPxycb_21 { z80->C = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SLA  C=(XY+o)    */
#define Z80_OPxycb_22 { z80->D = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SLA  D=(XY+o)    */
#define Z80_OPxycb_23 { z80->E = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SLA  E=(XY+o)    */
#define Z80_OPxycb_24 { z80->H = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SLA  H=(XY+o)    */
#define Z80_OPxycb_25 { z80->L = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SLA  L=(XY+o)    */
#define Z80_OPxycb_26 { WM(z80, z80->ea,SLA(z80, RM(z80, z80->ea)));							} /* SLA  (XY+o)      */
#define Z80_OPxycb_27 { z80->A = SLA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SLA  A=(XY+o)    */

#define Z80_OPxycb_28 { z80->B = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SRA  B=(XY+o)    */
#define Z80_OPxycb_29 { z80->C = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SRA  C=(XY+o)    */
#define Z80_OPxycb_2a { z80->D = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SRA  D=(XY+o)    */
#define Z80_OPxycb_2b { z80->E = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SRA  E=(XY+o)    */
#define Z80_OPxycb_2c { z80->H = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SRA  H=(XY+o)    */
#define Z80_OPxycb_2d { z80->L = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SRA  L=(XY+o)    */
#define Z80_OPxycb_2e { WM(z80, z80->ea,SRA(z80, RM(z80, z80->ea)));							} /* SRA  (XY+o)      */
#define Z80_OPxycb_2f { z80->A = SRA(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SRA  A=(XY+o)    */

#define Z80_OPxycb_30 { z80->B = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SLL  B=(XY+o)    */
#define Z80_OPxycb_31 { z80->C = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SLL  C=(XY+o)    */
#define Z80_OPxycb_32 { z80->D = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SLL  D=(XY+o)    */
#define Z80_OPxycb_33 { z80->E = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SLL  E=(XY+o)    */
#define Z80_OPxycb_34 { z80->H = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SLL  H=(XY+o)    */
#define Z80_OPxycb_35 { z80->L = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SLL  L=(XY+o)    */
#define Z80_OPxycb_36 { WM(z80, z80->ea,SLL(z80, RM(z80, z80->ea)));							} /* SLL  (XY+o)      */
#define Z80_OPxycb_37 { z80->A = SLL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SLL  A=(XY+o)    */

#define Z80_OPxycb_38 { z80->B = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SRL  B=(XY+o)    */
#define Z80_OPxycb_39 { z80->C = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SRL  C=(XY+o)    */
#define Z80_OPxycb_3a { z80->D = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SRL  D=(XY+o)    */
#define Z80_OPxycb_3b { z80->E = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SRL  E=(XY+o)    */
#define Z80_OPxycb_3c { z80->H = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SRL  H=(XY+o)    */
#define Z80_OPxycb_3d { z80->L = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SRL  L=(XY+o)    */
#define Z80_OPxycb_3e { WM(z80, z80->ea,SRL(z80, RM(z80, z80->ea)));							} /* SRL  (XY+o)      */
#define Z80_OPxycb_3f { z80->A = SRL(z80, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SRL  A=(XY+o)    */

#define Z80_OPxycb_40 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_41 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_42 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_43 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_44 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_45 { xycb_46(z80);															} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_46 { BIT_XY(z80, 0, RM(z80, z80->ea));										} /* BIT  0,(XY+o)    */
#define Z80_OPxycb_47 { xycb_46(z80);															} /* BIT  0,(XY+o)    */

#define Z80_OPxycb_48 { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_49 { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4a { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4b { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4c { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4d { xycb_4e(z80);															} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4e { BIT_XY(z80, 1, RM(z80, z80->ea));										} /* BIT  1,(XY+o)    */
#define Z80_OPxycb_4f { xycb_4e(z80);															} /* BIT  1,(XY+o)    */

#define Z80_OPxycb_50 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_51 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_52 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_53 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_54 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_55 { xycb_56(z80);															} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_56 { BIT_XY(z80, 2, RM(z80, z80->ea));										} /* BIT  2,(XY+o)    */
#define Z80_OPxycb_57 { xycb_56(z80);															} /* BIT  2,(XY+o)    */

#define Z80_OPxycb_58 { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_59 { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5a { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5b { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5c { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5d { xycb_5e(z80);															} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5e { BIT_XY(z80, 3, RM(z80, z80->ea));										} /* BIT  3,(XY+o)    */
#define Z80_OPxycb_5f { xycb_5e(z80);															} /* BIT  3,(XY+o)    */

#define Z80_OPxycb_60 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_61 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_62 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_63 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_64 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_65 { xycb_66(z80);															} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_66 { BIT_XY(z80, 4, RM(z80, z80->ea));										} /* BIT  4,(XY+o)    */
#define Z80_OPxycb_67 { xycb_66(z80);															} /* BIT  4,(XY+o)    */

#define Z80_OPxycb_68 { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_69 { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6a { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6b { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6c { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6d { xycb_6e(z80);															} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6e { BIT_XY(z80, 5, RM(z80, z80->ea));										} /* BIT  5,(XY+o)    */
#define Z80_OPxycb_6f { xycb_6e(z80);															} /* BIT  5,(XY+o)    */

#define Z80_OPxycb_70 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_71 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_72 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_73 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_74 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_75 { xycb_76(z80);															} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_76 { BIT_XY(z80, 6, RM(z80, z80->ea));										} /* BIT  6,(XY+o)    */
#define Z80_OPxycb_77 { xycb_76(z80);															} /* BIT  6,(XY+o)    */

#define Z80_OPxycb_78 { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_79 { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7a { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7b { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7c { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7d { xycb_7e(z80);															} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7e { BIT_XY(z80, 7, RM(z80, z80->ea));										} /* BIT  7,(XY+o)    */
#define Z80_OPxycb_7f { xycb_7e(z80);															} /* BIT  7,(XY+o)    */

#define Z80_OPxycb_80 { z80->B = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  0,B=(XY+o)  */
#define Z80_OPxycb_81 { z80->C = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  0,C=(XY+o)  */
#define Z80_OPxycb_82 { z80->D = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  0,D=(XY+o)  */
#define Z80_OPxycb_83 { z80->E = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  0,E=(XY+o)  */
#define Z80_OPxycb_84 { z80->H = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  0,H=(XY+o)  */
#define Z80_OPxycb_85 { z80->L = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  0,L=(XY+o)  */
#define Z80_OPxycb_86 { WM(z80, z80->ea, RES(0, RM(z80, z80->ea)));							} /* RES  0,(XY+o)    */
#define Z80_OPxycb_87 { z80->A = RES(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  0,A=(XY+o)  */

#define Z80_OPxycb_88 { z80->B = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  1,B=(XY+o)  */
#define Z80_OPxycb_89 { z80->C = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  1,C=(XY+o)  */
#define Z80_OPxycb_8a { z80->D = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  1,D=(XY+o)  */
#define Z80_OPxycb_8b { z80->E = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  1,E=(XY+o)  */
#define Z80_OPxycb_8c { z80->H = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  1,H=(XY+o)  */
#define Z80_OPxycb_8d { z80->L = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  1,L=(XY+o)  */
#define Z80_OPxycb_8e { WM(z80, z80->ea, RES(1, RM(z80, z80->ea)));							} /* RES  1,(XY+o)    */
#define Z80_OPxycb_8f { z80->A = RES(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  1,A=(XY+o)  */

#define Z80_OPxycb_90 { z80->B = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  2,B=(XY+o)  */
#define Z80_OPxycb_91 { z80->C = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  2,C=(XY+o)  */
#define Z80_OPxycb_92 { z80->D = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  2,D=(XY+o)  */
#define Z80_OPxycb_93 { z80->E = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  2,E=(XY+o)  */
#define Z80_OPxycb_94 { z80->H = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  2,H=(XY+o)  */
#define Z80_OPxycb_95 { z80->L = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  2,L=(XY+o)  */
#define Z80_OPxycb_96 { WM(z80, z80->ea, RES(2, RM(z80, z80->ea)));							} /* RES  2,(XY+o)    */
#define Z80_OPxycb_97 { z80->A = RES(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  2,A=(XY+o)  */

#define Z80_OPxycb_98 { z80->B = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  3,B=(XY+o)  */
#define Z80_OPxycb_99 { z80->C = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  3,C=(XY+o)  */
#define Z80_OPxycb_9a { z80->D = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  3,D=(XY+o)  */
#define Z80_OPxycb_9b { z80->E = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  3,E=(XY+o)  */
#define Z80_OPxycb_9c { z80->H = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  3,H=(XY+o)  */
#define Z80_OPxycb_9d { z80->L = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  3,L=(XY+o)  */
#define Z80_OPxycb_9e { WM(z80, z80->ea, RES(3, RM(z80, z80->ea)));							} /* RES  3,(XY+o)    */
#define Z80_OPxycb_9f { z80->A = RES(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  3,A=(XY+o)  */

#define Z80_OPxycb_a0 { z80->B = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  4,B=(XY+o)  */
#define Z80_OPxycb_a1 { z80->C = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  4,C=(XY+o)  */
#define Z80_OPxycb_a2 { z80->D = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  4,D=(XY+o)  */
#define Z80_OPxycb_a3 { z80->E = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  4,E=(XY+o)  */
#define Z80_OPxycb_a4 { z80->H = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  4,H=(XY+o)  */
#define Z80_OPxycb_a5 { z80->L = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  4,L=(XY+o)  */
#define Z80_OPxycb_a6 { WM(z80, z80->ea, RES(4, RM(z80, z80->ea)));							} /* RES  4,(XY+o)    */
#define Z80_OPxycb_a7 { z80->A = RES(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  4,A=(XY+o)  */

#define Z80_OPxycb_a8 { z80->B = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  5,B=(XY+o)  */
#define Z80_OPxycb_a9 { z80->C = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  5,C=(XY+o)  */
#define Z80_OPxycb_aa { z80->D = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  5,D=(XY+o)  */
#define Z80_OPxycb_ab { z80->E = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  5,E=(XY+o)  */
#define Z80_OPxycb_ac { z80->H = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  5,H=(XY+o)  */
#define Z80_OPxycb_ad { z80->L = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  5,L=(XY+o)  */
#define Z80_OPxycb_ae { WM(z80, z80->ea, RES(5, RM(z80, z80->ea)));							} /* RES  5,(XY+o)    */
#define Z80_OPxycb_af { z80->A = RES(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  5,A=(XY+o)  */

#define Z80_OPxycb_b0 { z80->B = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  6,B=(XY+o)  */
#define Z80_OPxycb_b1 { z80->C = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  6,C=(XY+o)  */
#define Z80_OPxycb_b2 { z80->D = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  6,D=(XY+o)  */
#define Z80_OPxycb_b3 { z80->E = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  6,E=(XY+o)  */
#define Z80_OPxycb_b4 { z80->H = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  6,H=(XY+o)  */
#define Z80_OPxycb_b5 { z80->L = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  6,L=(XY+o)  */
#define Z80_OPxycb_b6 { WM(z80, z80->ea, RES(6, RM(z80, z80->ea)));							} /* RES  6,(XY+o)    */
#define Z80_OPxycb_b7 { z80->A = RES(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  6,A=(XY+o)  */

#define Z80_OPxycb_b8 { z80->B = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* RES  7,B=(XY+o)  */
#define Z80_OPxycb_b9 { z80->C = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* RES  7,C=(XY+o)  */
#define Z80_OPxycb_ba { z80->D = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* RES  7,D=(XY+o)  */
#define Z80_OPxycb_bb { z80->E = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* RES  7,E=(XY+o)  */
#define Z80_OPxycb_bc { z80->H = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* RES  7,H=(XY+o)  */
#define Z80_OPxycb_bd { z80->L = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* RES  7,L=(XY+o)  */
#define Z80_OPxycb_be { WM(z80, z80->ea, RES(7, RM(z80, z80->ea)));							} /* RES  7,(XY+o)    */
#define Z80_OPxycb_bf { z80->A = RES(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* RES  7,A=(XY+o)  */

#define Z80_OPxycb_c0 { z80->B = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  0,B=(XY+o)  */
#define Z80_OPxycb_c1 { z80->C = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  0,C=(XY+o)  */
#define Z80_OPxycb_c2 { z80->D = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  0,D=(XY+o)  */
#define Z80_OPxycb_c3 { z80->E = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  0,E=(XY+o)  */
#define Z80_OPxycb_c4 { z80->H = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  0,H=(XY+o)  */
#define Z80_OPxycb_c5 { z80->L = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  0,L=(XY+o)  */
#define Z80_OPxycb_c6 { WM(z80, z80->ea, SET(0, RM(z80, z80->ea)));							} /* SET  0,(XY+o)    */
#define Z80_OPxycb_c7 { z80->A = SET(0, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  0,A=(XY+o)  */

#define Z80_OPxycb_c8 { z80->B = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  1,B=(XY+o)  */
#define Z80_OPxycb_c9 { z80->C = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  1,C=(XY+o)  */
#define Z80_OPxycb_ca { z80->D = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  1,D=(XY+o)  */
#define Z80_OPxycb_cb { z80->E = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  1,E=(XY+o)  */
#define Z80_OPxycb_cc { z80->H = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  1,H=(XY+o)  */
#define Z80_OPxycb_cd { z80->L = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  1,L=(XY+o)  */
#define Z80_OPxycb_ce { WM(z80, z80->ea, SET(1, RM(z80, z80->ea)));							} /* SET  1,(XY+o)    */
#define Z80_OPxycb_cf { z80->A = SET(1, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  1,A=(XY+o)  */

#define Z80_OPxycb_d0 { z80->B = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  2,B=(XY+o)  */
#define Z80_OPxycb_d1 { z80->C = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  2,C=(XY+o)  */
#define Z80_OPxycb_d2 { z80->D = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  2,D=(XY+o)  */
#define Z80_OPxycb_d3 { z80->E = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  2,E=(XY+o)  */
#define Z80_OPxycb_d4 { z80->H = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  2,H=(XY+o)  */
#define Z80_OPxycb_d5 { z80->L = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  2,L=(XY+o)  */
#define Z80_OPxycb_d6 { WM(z80, z80->ea, SET(2, RM(z80, z80->ea)));							} /* SET  2,(XY+o)    */
#define Z80_OPxycb_d7 { z80->A = SET(2, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  2,A=(XY+o)  */

#define Z80_OPxycb_d8 { z80->B = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  3,B=(XY+o)  */
#define Z80_OPxycb_d9 { z80->C = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  3,C=(XY+o)  */
#define Z80_OPxycb_da { z80->D = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  3,D=(XY+o)  */
#define Z80_OPxycb_db { z80->E = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  3,E=(XY+o)  */
#define Z80_OPxycb_dc { z80->H = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  3,H=(XY+o)  */
#define Z80_OPxycb_dd { z80->L = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  3,L=(XY+o)  */
#define Z80_OPxycb_de { WM(z80, z80->ea, SET(3, RM(z80, z80->ea)));							} /* SET  3,(XY+o)    */
#define Z80_OPxycb_df { z80->A = SET(3, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  3,A=(XY+o)  */

#define Z80_OPxycb_e0 { z80->B = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  4,B=(XY+o)  */
#define Z80_OPxycb_e1 { z80->C = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  4,C=(XY+o)  */
#define Z80_OPxycb_e2 { z80->D = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  4,D=(XY+o)  */
#define Z80_OPxycb_e3 { z80->E = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  4,E=(XY+o)  */
#define Z80_OPxycb_e4 { z80->H = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  4,H=(XY+o)  */
#define Z80_OPxycb_e5 { z80->L = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  4,L=(XY+o)  */
#define Z80_OPxycb_e6 { WM(z80, z80->ea, SET(4, RM(z80, z80->ea)));							} /* SET  4,(XY+o)    */
#define Z80_OPxycb_e7 { z80->A = SET(4, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  4,A=(XY+o)  */

#define Z80_OPxycb_e8 { z80->B = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  5,B=(XY+o)  */
#define Z80_OPxycb_e9 { z80->C = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  5,C=(XY+o)  */
#define Z80_OPxycb_ea { z80->D = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  5,D=(XY+o)  */
#define Z80_OPxycb_eb { z80->E = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  5,E=(XY+o)  */
#define Z80_OPxycb_ec { z80->H = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  5,H=(XY+o)  */
#define Z80_OPxycb_ed { z80->L = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  5,L=(XY+o)  */
#define Z80_OPxycb_ee { WM(z80, z80->ea, SET(5, RM(z80, z80->ea)));							} /* SET  5,(XY+o)    */
#define Z80_OPxycb_ef { z80->A = SET(5, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  5,A=(XY+o)  */

#define Z80_OPxycb_f0 { z80->B = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  6,B=(XY+o)  */
#define Z80_OPxycb_f1 { z80->C = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  6,C=(XY+o)  */
#define Z80_OPxycb_f2 { z80->D = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  6,D=(XY+o)  */
#define Z80_OPxycb_f3 { z80->E = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  6,E=(XY+o)  */
#define Z80_OPxycb_f4 { z80->H = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  6,H=(XY+o)  */
#define Z80_OPxycb_f5 { z80->L = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  6,L=(XY+o)  */
#define Z80_OPxycb_f6 { WM(z80, z80->ea, SET(6, RM(z80, z80->ea)));							} /* SET  6,(XY+o)    */
#define Z80_OPxycb_f7 { z80->A = SET(6, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  6,A=(XY+o)  */

#define Z80_OPxycb_f8 { z80->B = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->B);			} /* SET  7,B=(XY+o)  */
#define Z80_OPxycb_f9 { z80->C = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->C);			} /* SET  7,C=(XY+o)  */
#define Z80_OPxycb_fa { z80->D = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->D);			} /* SET  7,D=(XY+o)  */
#define Z80_OPxycb_fb { z80->E = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->E);			} /* SET  7,E=(XY+o)  */
#define Z80_OPxycb_fc { z80->H = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->H);			} /* SET  7,H=(XY+o)  */
#define Z80_OPxycb_fd { z80->L = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->L);			} /* SET  7,L=(XY+o)  */
#define Z80_OPxycb_fe { WM(z80, z80->ea, SET(7, RM(z80, z80->ea)));							} /* SET  7,(XY+o)    */
#define Z80_OPxycb_ff { z80->A = SET(7, RM(z80, z80->ea)); WM(z80, z80->ea,z80->A);			} /* SET  7,A=(XY+o)  */

#define Z80_OPillegal,1 {
	OELog("Z80 '%s' ill. opcode $%02x $%02x\n",
		  z80->device->tag, memory_decrypted_read_byte(z80->program, (z80->PCD-1)&0xffff), memory_decrypted_read_byte(z80->program, z80->PCD));
}

/**********************************************************
 * IX register related opcodes (DD prefix)
 **********************************************************/
#define Z80_OPdd,00 { illegal_1(z80); op_00(z80);												} /* DB   DD          */
#define Z80_OPdd,01 { illegal_1(z80); op_01(z80);												} /* DB   DD          */
#define Z80_OPdd,02 { illegal_1(z80); op_02(z80);												} /* DB   DD          */
#define Z80_OPdd,03 { illegal_1(z80); op_03(z80);												} /* DB   DD          */
#define Z80_OPdd,04 { illegal_1(z80); op_04(z80);												} /* DB   DD          */
#define Z80_OPdd,05 { illegal_1(z80); op_05(z80);												} /* DB   DD          */
#define Z80_OPdd,06 { illegal_1(z80); op_06(z80);												} /* DB   DD          */
#define Z80_OPdd,07 { illegal_1(z80); op_07(z80);												} /* DB   DD          */

#define Z80_OPdd,08 { illegal_1(z80); op_08(z80);												} /* DB   DD          */
#define Z80_OPdd,09 { ADD16(z80, ix, bc);														} /* ADD  IX,BC       */
#define Z80_OPdd,0a { illegal_1(z80); op_0a(z80);												} /* DB   DD          */
#define Z80_OPdd,0b { illegal_1(z80); op_0b(z80);												} /* DB   DD          */
#define Z80_OPdd,0c { illegal_1(z80); op_0c(z80);												} /* DB   DD          */
#define Z80_OPdd,0d { illegal_1(z80); op_0d(z80);												} /* DB   DD          */
#define Z80_OPdd,0e { illegal_1(z80); op_0e(z80);												} /* DB   DD          */
#define Z80_OPdd,0f { illegal_1(z80); op_0f(z80);												} /* DB   DD          */

#define Z80_OPdd,10 { illegal_1(z80); op_10(z80);												} /* DB   DD          */
#define Z80_OPdd,11 { illegal_1(z80); op_11(z80);												} /* DB   DD          */
#define Z80_OPdd,12 { illegal_1(z80); op_12(z80);												} /* DB   DD          */
#define Z80_OPdd,13 { illegal_1(z80); op_13(z80);												} /* DB   DD          */
#define Z80_OPdd,14 { illegal_1(z80); op_14(z80);												} /* DB   DD          */
#define Z80_OPdd,15 { illegal_1(z80); op_15(z80);												} /* DB   DD          */
#define Z80_OPdd,16 { illegal_1(z80); op_16(z80);												} /* DB   DD          */
#define Z80_OPdd,17 { illegal_1(z80); op_17(z80);												} /* DB   DD          */

#define Z80_OPdd,18 { illegal_1(z80); op_18(z80);												} /* DB   DD          */
#define Z80_OPdd,19 { ADD16(z80, ix, de);														} /* ADD  IX,DE       */
#define Z80_OPdd,1a { illegal_1(z80); op_1a(z80);												} /* DB   DD          */
#define Z80_OPdd,1b { illegal_1(z80); op_1b(z80);												} /* DB   DD          */
#define Z80_OPdd,1c { illegal_1(z80); op_1c(z80);												} /* DB   DD          */
#define Z80_OPdd,1d { illegal_1(z80); op_1d(z80);												} /* DB   DD          */
#define Z80_OPdd,1e { illegal_1(z80); op_1e(z80);												} /* DB   DD          */
#define Z80_OPdd,1f { illegal_1(z80); op_1f(z80);												} /* DB   DD          */

#define Z80_OPdd,20 { illegal_1(z80); op_20(z80);												} /* DB   DD          */
#define Z80_OPdd,21 { z80->IX = ARG16(z80);													} /* LD   IX,w        */
#define Z80_OPdd,22 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->ix);	z80->WZ = z80->ea+1;} /* LD   (w),IX      */
#define Z80_OPdd,23 { z80->IX++;																} /* INC  IX          */
#define Z80_OPdd,24 { z80->HX = INC(z80, z80->HX);											} /* INC  HX          */
#define Z80_OPdd,25 { z80->HX = DEC(z80, z80->HX);											} /* DEC  HX          */
#define Z80_OPdd,26 { z80->HX = ARG(z80);														} /* LD   HX,n        */
#define Z80_OPdd,27 { illegal_1(z80); op_27(z80);												} /* DB   DD          */

#define Z80_OPdd,28 { illegal_1(z80); op_28(z80);												} /* DB   DD          */
#define Z80_OPdd,29 { ADD16(z80, ix, ix);														} /* ADD  IX,IX       */
#define Z80_OPdd,2a { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->ix);	z80->WZ = z80->ea+1;} /* LD   IX,(w)      */
#define Z80_OPdd,2b { z80->IX--;																} /* DEC  IX          */
#define Z80_OPdd,2c { z80->LX = INC(z80, z80->LX);											} /* INC  LX          */
#define Z80_OPdd,2d { z80->LX = DEC(z80, z80->LX);											} /* DEC  LX          */
#define Z80_OPdd,2e { z80->LX = ARG(z80);														} /* LD   LX,n        */
#define Z80_OPdd,2f { illegal_1(z80); op_2f(z80);												} /* DB   DD          */

#define Z80_OPdd,30 { illegal_1(z80); op_30(z80);												} /* DB   DD          */
#define Z80_OPdd,31 { illegal_1(z80); op_31(z80);												} /* DB   DD          */
#define Z80_OPdd,32 { illegal_1(z80); op_32(z80);												} /* DB   DD          */
#define Z80_OPdd,33 { illegal_1(z80); op_33(z80);												} /* DB   DD          */
#define Z80_OPdd,34 { EAX(z80); WM(z80, z80->ea, INC(z80, RM(z80, z80->ea)));					} /* INC  (IX+o)      */
#define Z80_OPdd,35 { EAX(z80); WM(z80, z80->ea, DEC(z80, RM(z80, z80->ea)));					} /* DEC  (IX+o)      */
#define Z80_OPdd,36 { EAX(z80); WM(z80, z80->ea, ARG(z80));									} /* LD   (IX+o),n    */
#define Z80_OPdd,37 { illegal_1(z80); op_37(z80);												} /* DB   DD          */

#define Z80_OPdd,38 { illegal_1(z80); op_38(z80);												} /* DB   DD          */
#define Z80_OPdd,39 { ADD16(z80, ix, sp);														} /* ADD  IX,SP       */
#define Z80_OPdd,3a { illegal_1(z80); op_3a(z80);												} /* DB   DD          */
#define Z80_OPdd,3b { illegal_1(z80); op_3b(z80);												} /* DB   DD          */
#define Z80_OPdd,3c { illegal_1(z80); op_3c(z80);												} /* DB   DD          */
#define Z80_OPdd,3d { illegal_1(z80); op_3d(z80);												} /* DB   DD          */
#define Z80_OPdd,3e { illegal_1(z80); op_3e(z80);												} /* DB   DD          */
#define Z80_OPdd,3f { illegal_1(z80); op_3f(z80);												} /* DB   DD          */

#define Z80_OPdd,40 { illegal_1(z80); op_40(z80);												} /* DB   DD          */
#define Z80_OPdd,41 { illegal_1(z80); op_41(z80);												} /* DB   DD          */
#define Z80_OPdd,42 { illegal_1(z80); op_42(z80);												} /* DB   DD          */
#define Z80_OPdd,43 { illegal_1(z80); op_43(z80);												} /* DB   DD          */
#define Z80_OPdd,44 { z80->B = z80->HX;														} /* LD   B,HX        */
#define Z80_OPdd,45 { z80->B = z80->LX;														} /* LD   B,LX        */
#define Z80_OPdd,46 { EAX(z80); z80->B = RM(z80, z80->ea);									} /* LD   B,(IX+o)    */
#define Z80_OPdd,47 { illegal_1(z80); op_47(z80);												} /* DB   DD          */

#define Z80_OPdd,48 { illegal_1(z80); op_48(z80);												} /* DB   DD          */
#define Z80_OPdd,49 { illegal_1(z80); op_49(z80);												} /* DB   DD          */
#define Z80_OPdd,4a { illegal_1(z80); op_4a(z80);												} /* DB   DD          */
#define Z80_OPdd,4b { illegal_1(z80); op_4b(z80);												} /* DB   DD          */
#define Z80_OPdd,4c { z80->C = z80->HX;														} /* LD   C,HX        */
#define Z80_OPdd,4d { z80->C = z80->LX;														} /* LD   C,LX        */
#define Z80_OPdd,4e { EAX(z80); z80->C = RM(z80, z80->ea);									} /* LD   C,(IX+o)    */
#define Z80_OPdd,4f { illegal_1(z80); op_4f(z80);												} /* DB   DD          */

#define Z80_OPdd,50 { illegal_1(z80); op_50(z80);												} /* DB   DD          */
#define Z80_OPdd,51 { illegal_1(z80); op_51(z80);												} /* DB   DD          */
#define Z80_OPdd,52 { illegal_1(z80); op_52(z80);												} /* DB   DD          */
#define Z80_OPdd,53 { illegal_1(z80); op_53(z80);												} /* DB   DD          */
#define Z80_OPdd,54 { z80->D = z80->HX;														} /* LD   D,HX        */
#define Z80_OPdd,55 { z80->D = z80->LX;														} /* LD   D,LX        */
#define Z80_OPdd,56 { EAX(z80); z80->D = RM(z80, z80->ea);									} /* LD   D,(IX+o)    */
#define Z80_OPdd,57 { illegal_1(z80); op_57(z80);												} /* DB   DD          */

#define Z80_OPdd,58 { illegal_1(z80); op_58(z80);												} /* DB   DD          */
#define Z80_OPdd,59 { illegal_1(z80); op_59(z80);												} /* DB   DD          */
#define Z80_OPdd,5a { illegal_1(z80); op_5a(z80);												} /* DB   DD          */
#define Z80_OPdd,5b { illegal_1(z80); op_5b(z80);												} /* DB   DD          */
#define Z80_OPdd,5c { z80->E = z80->HX;														} /* LD   E,HX        */
#define Z80_OPdd,5d { z80->E = z80->LX;														} /* LD   E,LX        */
#define Z80_OPdd,5e { EAX(z80); z80->E = RM(z80, z80->ea);									} /* LD   E,(IX+o)    */
#define Z80_OPdd,5f { illegal_1(z80); op_5f(z80);												} /* DB   DD          */

#define Z80_OPdd,60 { z80->HX = z80->B;														} /* LD   HX,B        */
#define Z80_OPdd,61 { z80->HX = z80->C;														} /* LD   HX,C        */
#define Z80_OPdd,62 { z80->HX = z80->D;														} /* LD   HX,D        */
#define Z80_OPdd,63 { z80->HX = z80->E;														} /* LD   HX,E        */
#define Z80_OPdd,64 {																			} /* LD   HX,HX       */
#define Z80_OPdd,65 { z80->HX = z80->LX;														} /* LD   HX,LX       */
#define Z80_OPdd,66 { EAX(z80); z80->H = RM(z80, z80->ea);									} /* LD   H,(IX+o)    */
#define Z80_OPdd,67 { z80->HX = z80->A;														} /* LD   HX,A        */

#define Z80_OPdd,68 { z80->LX = z80->B;														} /* LD   LX,B        */
#define Z80_OPdd,69 { z80->LX = z80->C;														} /* LD   LX,C        */
#define Z80_OPdd,6a { z80->LX = z80->D;														} /* LD   LX,D        */
#define Z80_OPdd,6b { z80->LX = z80->E;														} /* LD   LX,E        */
#define Z80_OPdd,6c { z80->LX = z80->HX;														} /* LD   LX,HX       */
#define Z80_OPdd,6d {																			} /* LD   LX,LX       */
#define Z80_OPdd,6e { EAX(z80); z80->L = RM(z80, z80->ea);									} /* LD   L,(IX+o)    */
#define Z80_OPdd,6f { z80->LX = z80->A;														} /* LD   LX,A        */

#define Z80_OPdd,70 { EAX(z80); WM(z80, z80->ea, z80->B);										} /* LD   (IX+o),B    */
#define Z80_OPdd,71 { EAX(z80); WM(z80, z80->ea, z80->C);										} /* LD   (IX+o),C    */
#define Z80_OPdd,72 { EAX(z80); WM(z80, z80->ea, z80->D);										} /* LD   (IX+o),D    */
#define Z80_OPdd,73 { EAX(z80); WM(z80, z80->ea, z80->E);										} /* LD   (IX+o),E    */
#define Z80_OPdd,74 { EAX(z80); WM(z80, z80->ea, z80->H);										} /* LD   (IX+o),H    */
#define Z80_OPdd,75 { EAX(z80); WM(z80, z80->ea, z80->L);										} /* LD   (IX+o),L    */
#define Z80_OPdd,76 { illegal_1(z80); op_76(z80);												} /* DB   DD          */
#define Z80_OPdd,77 { EAX(z80); WM(z80, z80->ea, z80->A);										} /* LD   (IX+o),A    */

#define Z80_OPdd,78 { illegal_1(z80); op_78(z80);												} /* DB   DD          */
#define Z80_OPdd,79 { illegal_1(z80); op_79(z80);												} /* DB   DD          */
#define Z80_OPdd,7a { illegal_1(z80); op_7a(z80);												} /* DB   DD          */
#define Z80_OPdd,7b { illegal_1(z80); op_7b(z80);												} /* DB   DD          */
#define Z80_OPdd,7c { z80->A = z80->HX;														} /* LD   A,HX        */
#define Z80_OPdd,7d { z80->A = z80->LX;														} /* LD   A,LX        */
#define Z80_OPdd,7e { EAX(z80); z80->A = RM(z80, z80->ea);									} /* LD   A,(IX+o)    */
#define Z80_OPdd,7f { illegal_1(z80); op_7f(z80);												} /* DB   DD          */

#define Z80_OPdd,80 { illegal_1(z80); op_80(z80);												} /* DB   DD          */
#define Z80_OPdd,81 { illegal_1(z80); op_81(z80);												} /* DB   DD          */
#define Z80_OPdd,82 { illegal_1(z80); op_82(z80);												} /* DB   DD          */
#define Z80_OPdd,83 { illegal_1(z80); op_83(z80);												} /* DB   DD          */
#define Z80_OPdd,84 { ADD(z80, z80->HX);														} /* ADD  A,HX        */
#define Z80_OPdd,85 { ADD(z80, z80->LX);														} /* ADD  A,LX        */
#define Z80_OPdd,86 { EAX(z80); ADD(z80, RM(z80, z80->ea));									} /* ADD  A,(IX+o)    */
#define Z80_OPdd,87 { illegal_1(z80); op_87(z80);												} /* DB   DD          */

#define Z80_OPdd,88 { illegal_1(z80); op_88(z80);												} /* DB   DD          */
#define Z80_OPdd,89 { illegal_1(z80); op_89(z80);												} /* DB   DD          */
#define Z80_OPdd,8a { illegal_1(z80); op_8a(z80);												} /* DB   DD          */
#define Z80_OPdd,8b { illegal_1(z80); op_8b(z80);												} /* DB   DD          */
#define Z80_OPdd,8c { ADC(z80, z80->HX);														} /* ADC  A,HX        */
#define Z80_OPdd,8d { ADC(z80, z80->LX);														} /* ADC  A,LX        */
#define Z80_OPdd,8e { EAX(z80); ADC(z80, RM(z80, z80->ea));									} /* ADC  A,(IX+o)    */
#define Z80_OPdd,8f { illegal_1(z80); op_8f(z80);												} /* DB   DD          */

#define Z80_OPdd,90 { illegal_1(z80); op_90(z80);												} /* DB   DD          */
#define Z80_OPdd,91 { illegal_1(z80); op_91(z80);												} /* DB   DD          */
#define Z80_OPdd,92 { illegal_1(z80); op_92(z80);												} /* DB   DD          */
#define Z80_OPdd,93 { illegal_1(z80); op_93(z80);												} /* DB   DD          */
#define Z80_OPdd,94 { SUB(z80, z80->HX);														} /* SUB  HX          */
#define Z80_OPdd,95 { SUB(z80, z80->LX);														} /* SUB  LX          */
#define Z80_OPdd,96 { EAX(z80); SUB(z80, RM(z80, z80->ea));									} /* SUB  (IX+o)      */
#define Z80_OPdd,97 { illegal_1(z80); op_97(z80);												} /* DB   DD          */

#define Z80_OPdd,98 { illegal_1(z80); op_98(z80);												} /* DB   DD          */
#define Z80_OPdd,99 { illegal_1(z80); op_99(z80);												} /* DB   DD          */
#define Z80_OPdd,9a { illegal_1(z80); op_9a(z80);												} /* DB   DD          */
#define Z80_OPdd,9b { illegal_1(z80); op_9b(z80);												} /* DB   DD          */
#define Z80_OPdd,9c { SBC(z80, z80->HX);														} /* SBC  A,HX        */
#define Z80_OPdd,9d { SBC(z80, z80->LX);														} /* SBC  A,LX        */
#define Z80_OPdd,9e { EAX(z80); SBC(z80, RM(z80, z80->ea));									} /* SBC  A,(IX+o)    */
#define Z80_OPdd,9f { illegal_1(z80); op_9f(z80);												} /* DB   DD          */

#define Z80_OPdd,a0 { illegal_1(z80); op_a0(z80);												} /* DB   DD          */
#define Z80_OPdd,a1 { illegal_1(z80); op_a1(z80);												} /* DB   DD          */
#define Z80_OPdd,a2 { illegal_1(z80); op_a2(z80);												} /* DB   DD          */
#define Z80_OPdd,a3 { illegal_1(z80); op_a3(z80);												} /* DB   DD          */
#define Z80_OPdd,a4 { AND(z80, z80->HX);														} /* AND  HX          */
#define Z80_OPdd,a5 { AND(z80, z80->LX);														} /* AND  LX          */
#define Z80_OPdd,a6 { EAX(z80); AND(z80, RM(z80, z80->ea));									} /* AND  (IX+o)      */
#define Z80_OPdd,a7 { illegal_1(z80); op_a7(z80);												} /* DB   DD          */

#define Z80_OPdd,a8 { illegal_1(z80); op_a8(z80);												} /* DB   DD          */
#define Z80_OPdd,a9 { illegal_1(z80); op_a9(z80);												} /* DB   DD          */
#define Z80_OPdd,aa { illegal_1(z80); op_aa(z80);												} /* DB   DD          */
#define Z80_OPdd,ab { illegal_1(z80); op_ab(z80);												} /* DB   DD          */
#define Z80_OPdd,ac { XOR(z80, z80->HX);														} /* XOR  HX          */
#define Z80_OPdd,ad { XOR(z80, z80->LX);														} /* XOR  LX          */
#define Z80_OPdd,ae { EAX(z80); XOR(z80, RM(z80, z80->ea));									} /* XOR  (IX+o)      */
#define Z80_OPdd,af { illegal_1(z80); op_af(z80);												} /* DB   DD          */

#define Z80_OPdd,b0 { illegal_1(z80); op_b0(z80);												} /* DB   DD          */
#define Z80_OPdd,b1 { illegal_1(z80); op_b1(z80);												} /* DB   DD          */
#define Z80_OPdd,b2 { illegal_1(z80); op_b2(z80);												} /* DB   DD          */
#define Z80_OPdd,b3 { illegal_1(z80); op_b3(z80);												} /* DB   DD          */
#define Z80_OPdd,b4 { OR(z80, z80->HX);														} /* OR   HX          */
#define Z80_OPdd,b5 { OR(z80, z80->LX);														} /* OR   LX          */
#define Z80_OPdd,b6 { EAX(z80); OR(z80, RM(z80, z80->ea));									} /* OR   (IX+o)      */
#define Z80_OPdd,b7 { illegal_1(z80); op_b7(z80);												} /* DB   DD          */

#define Z80_OPdd,b8 { illegal_1(z80); op_b8(z80);												} /* DB   DD          */
#define Z80_OPdd,b9 { illegal_1(z80); op_b9(z80);												} /* DB   DD          */
#define Z80_OPdd,ba { illegal_1(z80); op_ba(z80);												} /* DB   DD          */
#define Z80_OPdd,bb { illegal_1(z80); op_bb(z80);												} /* DB   DD          */
#define Z80_OPdd,bc { CP(z80, z80->HX);														} /* CP   HX          */
#define Z80_OPdd,bd { CP(z80, z80->LX);														} /* CP   LX          */
#define Z80_OPdd,be { EAX(z80); CP(z80, RM(z80, z80->ea));									} /* CP   (IX+o)      */
#define Z80_OPdd,bf { illegal_1(z80); op_bf(z80);												} /* DB   DD          */

#define Z80_OPdd,c0 { illegal_1(z80); op_c0(z80);												} /* DB   DD          */
#define Z80_OPdd,c1 { illegal_1(z80); op_c1(z80);												} /* DB   DD          */
#define Z80_OPdd,c2 { illegal_1(z80); op_c2(z80);												} /* DB   DD          */
#define Z80_OPdd,c3 { illegal_1(z80); op_c3(z80);												} /* DB   DD          */
#define Z80_OPdd,c4 { illegal_1(z80); op_c4(z80);												} /* DB   DD          */
#define Z80_OPdd,c5 { illegal_1(z80); op_c5(z80);												} /* DB   DD          */
#define Z80_OPdd,c6 { illegal_1(z80); op_c6(z80);												} /* DB   DD          */
#define Z80_OPdd,c7 { illegal_1(z80); op_c7(z80);												} /* DB   DD          */

#define Z80_OPdd,c8 { illegal_1(z80); op_c8(z80);												} /* DB   DD          */
#define Z80_OPdd,c9 { illegal_1(z80); op_c9(z80);												} /* DB   DD          */
#define Z80_OPdd,ca { illegal_1(z80); op_ca(z80);												} /* DB   DD          */
#define Z80_OPdd,cb { EAX(z80); EXEC(z80,xycb,ARG(z80));										} /* **   DD CB xx    */
#define Z80_OPdd,cc { illegal_1(z80); op_cc(z80);												} /* DB   DD          */
#define Z80_OPdd,cd { illegal_1(z80); op_cd(z80);												} /* DB   DD          */
#define Z80_OPdd,ce { illegal_1(z80); op_ce(z80);												} /* DB   DD          */
#define Z80_OPdd,cf { illegal_1(z80); op_cf(z80);												} /* DB   DD          */

#define Z80_OPdd,d0 { illegal_1(z80); op_d0(z80);												} /* DB   DD          */
#define Z80_OPdd,d1 { illegal_1(z80); op_d1(z80);												} /* DB   DD          */
#define Z80_OPdd,d2 { illegal_1(z80); op_d2(z80);												} /* DB   DD          */
#define Z80_OPdd,d3 { illegal_1(z80); op_d3(z80);												} /* DB   DD          */
#define Z80_OPdd,d4 { illegal_1(z80); op_d4(z80);												} /* DB   DD          */
#define Z80_OPdd,d5 { illegal_1(z80); op_d5(z80);												} /* DB   DD          */
#define Z80_OPdd,d6 { illegal_1(z80); op_d6(z80);												} /* DB   DD          */
#define Z80_OPdd,d7 { illegal_1(z80); op_d7(z80);												} /* DB   DD          */

#define Z80_OPdd,d8 { illegal_1(z80); op_d8(z80);												} /* DB   DD          */
#define Z80_OPdd,d9 { illegal_1(z80); op_d9(z80);												} /* DB   DD          */
#define Z80_OPdd,da { illegal_1(z80); op_da(z80);												} /* DB   DD          */
#define Z80_OPdd,db { illegal_1(z80); op_db(z80);												} /* DB   DD          */
#define Z80_OPdd,dc { illegal_1(z80); op_dc(z80);												} /* DB   DD          */
#define Z80_OPdd,dd { illegal_1(z80); op_dd(z80);												} /* DB   DD          */
#define Z80_OPdd,de { illegal_1(z80); op_de(z80);												} /* DB   DD          */
#define Z80_OPdd,df { illegal_1(z80); op_df(z80);												} /* DB   DD          */

#define Z80_OPdd,e0 { illegal_1(z80); op_e0(z80);												} /* DB   DD          */
#define Z80_OPdd,e1 { P#define Z80_OPz80, ix);															} /* POP  IX          */
#define Z80_OPdd,e2 { illegal_1(z80); op_e2(z80);												} /* DB   DD          */
#define Z80_OPdd,e3 { EXSP(z80, ix);															} /* EX   (SP),IX     */
#define Z80_OPdd,e4 { illegal_1(z80); op_e4(z80);												} /* DB   DD          */
#define Z80_OPdd,e5 { PUSH(z80, ix);															} /* PUSH IX          */
#define Z80_OPdd,e6 { illegal_1(z80); op_e6(z80);												} /* DB   DD          */
#define Z80_OPdd,e7 { illegal_1(z80); op_e7(z80);												} /* DB   DD          */

#define Z80_OPdd,e8 { illegal_1(z80); op_e8(z80);												} /* DB   DD          */
#define Z80_OPdd,e9 { z80->PC = z80->IX;														} /* JP   (IX)        */
#define Z80_OPdd,ea { illegal_1(z80); op_ea(z80);												} /* DB   DD          */
#define Z80_OPdd,eb { illegal_1(z80); op_eb(z80);												} /* DB   DD          */
#define Z80_OPdd,ec { illegal_1(z80); op_ec(z80);												} /* DB   DD          */
#define Z80_OPdd,ed { illegal_1(z80); op_ed(z80);												} /* DB   DD          */
#define Z80_OPdd,ee { illegal_1(z80); op_ee(z80);												} /* DB   DD          */
#define Z80_OPdd,ef { illegal_1(z80); op_ef(z80);												} /* DB   DD          */

#define Z80_OPdd,f0 { illegal_1(z80); op_f0(z80);												} /* DB   DD          */
#define Z80_OPdd,f1 { illegal_1(z80); op_f1(z80);												} /* DB   DD          */
#define Z80_OPdd,f2 { illegal_1(z80); op_f2(z80);												} /* DB   DD          */
#define Z80_OPdd,f3 { illegal_1(z80); op_f3(z80);												} /* DB   DD          */
#define Z80_OPdd,f4 { illegal_1(z80); op_f4(z80);												} /* DB   DD          */
#define Z80_OPdd,f5 { illegal_1(z80); op_f5(z80);												} /* DB   DD          */
#define Z80_OPdd,f6 { illegal_1(z80); op_f6(z80);												} /* DB   DD          */
#define Z80_OPdd,f7 { illegal_1(z80); op_f7(z80);												} /* DB   DD          */

#define Z80_OPdd,f8 { illegal_1(z80); op_f8(z80);												} /* DB   DD          */
#define Z80_OPdd,f9 { z80->SP = z80->IX;														} /* LD   SP,IX       */
#define Z80_OPdd,fa { illegal_1(z80); op_fa(z80);												} /* DB   DD          */
#define Z80_OPdd,fb { illegal_1(z80); op_fb(z80);												} /* DB   DD          */
#define Z80_OPdd,fc { illegal_1(z80); op_fc(z80);												} /* DB   DD          */
#define Z80_OPdd,fd { illegal_1(z80); op_fd(z80);												} /* DB   DD          */
#define Z80_OPdd,fe { illegal_1(z80); op_fe(z80);												} /* DB   DD          */
#define Z80_OPdd,ff { illegal_1(z80); op_ff(z80);												} /* DB   DD          */

/**********************************************************
 * IY register related opcodes (FD prefix)
 **********************************************************/
#define Z80_OPfd_00 { illegal_1(z80); op_00(z80);												} /* DB   FD          */
#define Z80_OPfd_01 { illegal_1(z80); op_01(z80);												} /* DB   FD          */
#define Z80_OPfd_02 { illegal_1(z80); op_02(z80);												} /* DB   FD          */
#define Z80_OPfd_03 { illegal_1(z80); op_03(z80);												} /* DB   FD          */
#define Z80_OPfd_04 { illegal_1(z80); op_04(z80);												} /* DB   FD          */
#define Z80_OPfd_05 { illegal_1(z80); op_05(z80);												} /* DB   FD          */
#define Z80_OPfd_06 { illegal_1(z80); op_06(z80);												} /* DB   FD          */
#define Z80_OPfd_07 { illegal_1(z80); op_07(z80);												} /* DB   FD          */

#define Z80_OPfd_08 { illegal_1(z80); op_08(z80);												} /* DB   FD          */
#define Z80_OPfd_09 { ADD16(z80, iy, bc);														} /* ADD  IY,BC       */
#define Z80_OPfd_0a { illegal_1(z80); op_0a(z80);												} /* DB   FD          */
#define Z80_OPfd_0b { illegal_1(z80); op_0b(z80);												} /* DB   FD          */
#define Z80_OPfd_0c { illegal_1(z80); op_0c(z80);												} /* DB   FD          */
#define Z80_OPfd_0d { illegal_1(z80); op_0d(z80);												} /* DB   FD          */
#define Z80_OPfd_0e { illegal_1(z80); op_0e(z80);												} /* DB   FD          */
#define Z80_OPfd_0f { illegal_1(z80); op_0f(z80);												} /* DB   FD          */

#define Z80_OPfd_10 { illegal_1(z80); op_10(z80);												} /* DB   FD          */
#define Z80_OPfd_11 { illegal_1(z80); op_11(z80);												} /* DB   FD          */
#define Z80_OPfd_12 { illegal_1(z80); op_12(z80);												} /* DB   FD          */
#define Z80_OPfd_13 { illegal_1(z80); op_13(z80);												} /* DB   FD          */
#define Z80_OPfd_14 { illegal_1(z80); op_14(z80);												} /* DB   FD          */
#define Z80_OPfd_15 { illegal_1(z80); op_15(z80);												} /* DB   FD          */
#define Z80_OPfd_16 { illegal_1(z80); op_16(z80);												} /* DB   FD          */
#define Z80_OPfd_17 { illegal_1(z80); op_17(z80);												} /* DB   FD          */

#define Z80_OPfd_18 { illegal_1(z80); op_18(z80);												} /* DB   FD          */
#define Z80_OPfd_19 { ADD16(z80, iy, de);														} /* ADD  IY,DE       */
#define Z80_OPfd_1a { illegal_1(z80); op_1a(z80);												} /* DB   FD          */
#define Z80_OPfd_1b { illegal_1(z80); op_1b(z80);												} /* DB   FD          */
#define Z80_OPfd_1c { illegal_1(z80); op_1c(z80);												} /* DB   FD          */
#define Z80_OPfd_1d { illegal_1(z80); op_1d(z80);												} /* DB   FD          */
#define Z80_OPfd_1e { illegal_1(z80); op_1e(z80);												} /* DB   FD          */
#define Z80_OPfd_1f { illegal_1(z80); op_1f(z80);												} /* DB   FD          */

#define Z80_OPfd_20 { illegal_1(z80); op_20(z80);												} /* DB   FD          */
#define Z80_OPfd_21 { z80->IY = ARG16(z80);													} /* LD   IY,w        */
#define Z80_OPfd_22 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->iy); z80->WZ = z80->ea+1;} /* LD   (w),IY      */
#define Z80_OPfd_23 { z80->IY++;																} /* INC  IY          */
#define Z80_OPfd_24 { z80->HY = INC(z80, z80->HY);											} /* INC  HY          */
#define Z80_OPfd_25 { z80->HY = DEC(z80, z80->HY);											} /* DEC  HY          */
#define Z80_OPfd_26 { z80->HY = ARG(z80);														} /* LD   HY,n        */
#define Z80_OPfd_27 { illegal_1(z80); op_27(z80);												} /* DB   FD          */

#define Z80_OPfd_28 { illegal_1(z80); op_28(z80);												} /* DB   FD          */
#define Z80_OPfd_29 { ADD16(z80, iy, iy);														} /* ADD  IY,IY       */
#define Z80_OPfd_2a { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->iy); z80->WZ = z80->ea+1;} /* LD   IY,(w)      */
#define Z80_OPfd_2b { z80->IY--;																} /* DEC  IY          */
#define Z80_OPfd_2c { z80->LY = INC(z80, z80->LY);											} /* INC  LY          */
#define Z80_OPfd_2d { z80->LY = DEC(z80, z80->LY);											} /* DEC  LY          */
#define Z80_OPfd_2e { z80->LY = ARG(z80);														} /* LD   LY,n        */
#define Z80_OPfd_2f { illegal_1(z80); op_2f(z80);												} /* DB   FD          */

#define Z80_OPfd_30 { illegal_1(z80); op_30(z80);												} /* DB   FD          */
#define Z80_OPfd_31 { illegal_1(z80); op_31(z80);												} /* DB   FD          */
#define Z80_OPfd_32 { illegal_1(z80); op_32(z80);												} /* DB   FD          */
#define Z80_OPfd_33 { illegal_1(z80); op_33(z80);												} /* DB   FD          */
#define Z80_OPfd_34 { EAY(z80); WM(z80, z80->ea, INC(z80, RM(z80, z80->ea)));					} /* INC  (IY+o)      */
#define Z80_OPfd_35 { EAY(z80); WM(z80, z80->ea, DEC(z80, RM(z80, z80->ea)));					} /* DEC  (IY+o)      */
#define Z80_OPfd_36 { EAY(z80); WM(z80, z80->ea, ARG(z80));									} /* LD   (IY+o),n    */
#define Z80_OPfd_37 { illegal_1(z80); op_37(z80);												} /* DB   FD          */

#define Z80_OPfd_38 { illegal_1(z80); op_38(z80);												} /* DB   FD          */
#define Z80_OPfd_39 { ADD16(z80, iy, sp);														} /* ADD  IY,SP       */
#define Z80_OPfd_3a { illegal_1(z80); op_3a(z80);												} /* DB   FD          */
#define Z80_OPfd_3b { illegal_1(z80); op_3b(z80);												} /* DB   FD          */
#define Z80_OPfd_3c { illegal_1(z80); op_3c(z80);												} /* DB   FD          */
#define Z80_OPfd_3d { illegal_1(z80); op_3d(z80);												} /* DB   FD          */
#define Z80_OPfd_3e { illegal_1(z80); op_3e(z80);												} /* DB   FD          */
#define Z80_OPfd_3f { illegal_1(z80); op_3f(z80);												} /* DB   FD          */

#define Z80_OPfd_40 { illegal_1(z80); op_40(z80);												} /* DB   FD          */
#define Z80_OPfd_41 { illegal_1(z80); op_41(z80);												} /* DB   FD          */
#define Z80_OPfd_42 { illegal_1(z80); op_42(z80);												} /* DB   FD          */
#define Z80_OPfd_43 { illegal_1(z80); op_43(z80);												} /* DB   FD          */
#define Z80_OPfd_44 { z80->B = z80->HY;														} /* LD   B,HY        */
#define Z80_OPfd_45 { z80->B = z80->LY;														} /* LD   B,LY        */
#define Z80_OPfd_46 { EAY(z80); z80->B = RM(z80, z80->ea);									} /* LD   B,(IY+o)    */
#define Z80_OPfd_47 { illegal_1(z80); op_47(z80);												} /* DB   FD          */

#define Z80_OPfd_48 { illegal_1(z80); op_48(z80);												} /* DB   FD          */
#define Z80_OPfd_49 { illegal_1(z80); op_49(z80);												} /* DB   FD          */
#define Z80_OPfd_4a { illegal_1(z80); op_4a(z80);												} /* DB   FD          */
#define Z80_OPfd_4b { illegal_1(z80); op_4b(z80);												} /* DB   FD          */
#define Z80_OPfd_4c { z80->C = z80->HY;														} /* LD   C,HY        */
#define Z80_OPfd_4d { z80->C = z80->LY;														} /* LD   C,LY        */
#define Z80_OPfd_4e { EAY(z80); z80->C = RM(z80, z80->ea);									} /* LD   C,(IY+o)    */
#define Z80_OPfd_4f { illegal_1(z80); op_4f(z80);												} /* DB   FD          */

#define Z80_OPfd_50 { illegal_1(z80); op_50(z80);												} /* DB   FD          */
#define Z80_OPfd_51 { illegal_1(z80); op_51(z80);												} /* DB   FD          */
#define Z80_OPfd_52 { illegal_1(z80); op_52(z80);												} /* DB   FD          */
#define Z80_OPfd_53 { illegal_1(z80); op_53(z80);												} /* DB   FD          */
#define Z80_OPfd_54 { z80->D = z80->HY;														} /* LD   D,HY        */
#define Z80_OPfd_55 { z80->D = z80->LY;														} /* LD   D,LY        */
#define Z80_OPfd_56 { EAY(z80); z80->D = RM(z80, z80->ea);									} /* LD   D,(IY+o)    */
#define Z80_OPfd_57 { illegal_1(z80); op_57(z80);												} /* DB   FD          */

#define Z80_OPfd_58 { illegal_1(z80); op_58(z80);												} /* DB   FD          */
#define Z80_OPfd_59 { illegal_1(z80); op_59(z80);												} /* DB   FD          */
#define Z80_OPfd_5a { illegal_1(z80); op_5a(z80);												} /* DB   FD          */
#define Z80_OPfd_5b { illegal_1(z80); op_5b(z80);												} /* DB   FD          */
#define Z80_OPfd_5c { z80->E = z80->HY;														} /* LD   E,HY        */
#define Z80_OPfd_5d { z80->E = z80->LY;														} /* LD   E,LY        */
#define Z80_OPfd_5e { EAY(z80); z80->E = RM(z80, z80->ea);									} /* LD   E,(IY+o)    */
#define Z80_OPfd_5f { illegal_1(z80); op_5f(z80);												} /* DB   FD          */

#define Z80_OPfd_60 { z80->HY = z80->B;														} /* LD   HY,B        */
#define Z80_OPfd_61 { z80->HY = z80->C;														} /* LD   HY,C        */
#define Z80_OPfd_62 { z80->HY = z80->D;														} /* LD   HY,D        */
#define Z80_OPfd_63 { z80->HY = z80->E;														} /* LD   HY,E        */
#define Z80_OPfd_64 {																			} /* LD   HY,HY       */
#define Z80_OPfd_65 { z80->HY = z80->LY;														} /* LD   HY,LY       */
#define Z80_OPfd_66 { EAY(z80); z80->H = RM(z80, z80->ea);									} /* LD   H,(IY+o)    */
#define Z80_OPfd_67 { z80->HY = z80->A;														} /* LD   HY,A        */

#define Z80_OPfd_68 { z80->LY = z80->B;														} /* LD   LY,B        */
#define Z80_OPfd_69 { z80->LY = z80->C;														} /* LD   LY,C        */
#define Z80_OPfd_6a { z80->LY = z80->D;														} /* LD   LY,D        */
#define Z80_OPfd_6b { z80->LY = z80->E;														} /* LD   LY,E        */
#define Z80_OPfd_6c { z80->LY = z80->HY;														} /* LD   LY,HY       */
#define Z80_OPfd_6d {																			} /* LD   LY,LY       */
#define Z80_OPfd_6e { EAY(z80); z80->L = RM(z80, z80->ea);									} /* LD   L,(IY+o)    */
#define Z80_OPfd_6f { z80->LY = z80->A;														} /* LD   LY,A        */

#define Z80_OPfd_70 { EAY(z80); WM(z80, z80->ea, z80->B);										} /* LD   (IY+o),B    */
#define Z80_OPfd_71 { EAY(z80); WM(z80, z80->ea, z80->C);										} /* LD   (IY+o),C    */
#define Z80_OPfd_72 { EAY(z80); WM(z80, z80->ea, z80->D);										} /* LD   (IY+o),D    */
#define Z80_OPfd_73 { EAY(z80); WM(z80, z80->ea, z80->E);										} /* LD   (IY+o),E    */
#define Z80_OPfd_74 { EAY(z80); WM(z80, z80->ea, z80->H);										} /* LD   (IY+o),H    */
#define Z80_OPfd_75 { EAY(z80); WM(z80, z80->ea, z80->L);										} /* LD   (IY+o),L    */
#define Z80_OPfd_76 { illegal_1(z80); op_76(z80);												} /* DB   FD          */
#define Z80_OPfd_77 { EAY(z80); WM(z80, z80->ea, z80->A);										} /* LD   (IY+o),A    */

#define Z80_OPfd_78 { illegal_1(z80); op_78(z80);												} /* DB   FD          */
#define Z80_OPfd_79 { illegal_1(z80); op_79(z80);												} /* DB   FD          */
#define Z80_OPfd_7a { illegal_1(z80); op_7a(z80);												} /* DB   FD          */
#define Z80_OPfd_7b { illegal_1(z80); op_7b(z80);												} /* DB   FD          */
#define Z80_OPfd_7c { z80->A = z80->HY;														} /* LD   A,HY        */
#define Z80_OPfd_7d { z80->A = z80->LY;														} /* LD   A,LY        */
#define Z80_OPfd_7e { EAY(z80); z80->A = RM(z80, z80->ea);									} /* LD   A,(IY+o)    */
#define Z80_OPfd_7f { illegal_1(z80); op_7f(z80);												} /* DB   FD          */

#define Z80_OPfd_80 { illegal_1(z80); op_80(z80);												} /* DB   FD          */
#define Z80_OPfd_81 { illegal_1(z80); op_81(z80);												} /* DB   FD          */
#define Z80_OPfd_82 { illegal_1(z80); op_82(z80);												} /* DB   FD          */
#define Z80_OPfd_83 { illegal_1(z80); op_83(z80);												} /* DB   FD          */
#define Z80_OPfd_84 { ADD(z80, z80->HY);														} /* ADD  A,HY        */
#define Z80_OPfd_85 { ADD(z80, z80->LY);														} /* ADD  A,LY        */
#define Z80_OPfd_86 { EAY(z80); ADD(z80, RM(z80, z80->ea));									} /* ADD  A,(IY+o)    */
#define Z80_OPfd_87 { illegal_1(z80); op_87(z80);												} /* DB   FD          */

#define Z80_OPfd_88 { illegal_1(z80); op_88(z80);												} /* DB   FD          */
#define Z80_OPfd_89 { illegal_1(z80); op_89(z80);												} /* DB   FD          */
#define Z80_OPfd_8a { illegal_1(z80); op_8a(z80);												} /* DB   FD          */
#define Z80_OPfd_8b { illegal_1(z80); op_8b(z80);												} /* DB   FD          */
#define Z80_OPfd_8c { ADC(z80, z80->HY);														} /* ADC  A,HY        */
#define Z80_OPfd_8d { ADC(z80, z80->LY);														} /* ADC  A,LY        */
#define Z80_OPfd_8e { EAY(z80); ADC(z80, RM(z80, z80->ea));									} /* ADC  A,(IY+o)    */
#define Z80_OPfd_8f { illegal_1(z80); op_8f(z80);												} /* DB   FD          */

#define Z80_OPfd_90 { illegal_1(z80); op_90(z80);												} /* DB   FD          */
#define Z80_OPfd_91 { illegal_1(z80); op_91(z80);												} /* DB   FD          */
#define Z80_OPfd_92 { illegal_1(z80); op_92(z80);												} /* DB   FD          */
#define Z80_OPfd_93 { illegal_1(z80); op_93(z80);												} /* DB   FD          */
#define Z80_OPfd_94 { SUB(z80, z80->HY);														} /* SUB  HY          */
#define Z80_OPfd_95 { SUB(z80, z80->LY);														} /* SUB  LY          */
#define Z80_OPfd_96 { EAY(z80); SUB(z80, RM(z80, z80->ea));									} /* SUB  (IY+o)      */
#define Z80_OPfd_97 { illegal_1(z80); op_97(z80);												} /* DB   FD          */

#define Z80_OPfd_98 { illegal_1(z80); op_98(z80);												} /* DB   FD          */
#define Z80_OPfd_99 { illegal_1(z80); op_99(z80);												} /* DB   FD          */
#define Z80_OPfd_9a { illegal_1(z80); op_9a(z80);												} /* DB   FD          */
#define Z80_OPfd_9b { illegal_1(z80); op_9b(z80);												} /* DB   FD          */
#define Z80_OPfd_9c { SBC(z80, z80->HY);														} /* SBC  A,HY        */
#define Z80_OPfd_9d { SBC(z80, z80->LY);														} /* SBC  A,LY        */
#define Z80_OPfd_9e { EAY(z80); SBC(z80, RM(z80, z80->ea));									} /* SBC  A,(IY+o)    */
#define Z80_OPfd_9f { illegal_1(z80); op_9f(z80);												} /* DB   FD          */

#define Z80_OPfd_a0 { illegal_1(z80); op_a0(z80);												} /* DB   FD          */
#define Z80_OPfd_a1 { illegal_1(z80); op_a1(z80);												} /* DB   FD          */
#define Z80_OPfd_a2 { illegal_1(z80); op_a2(z80);												} /* DB   FD          */
#define Z80_OPfd_a3 { illegal_1(z80); op_a3(z80);												} /* DB   FD          */
#define Z80_OPfd_a4 { AND(z80, z80->HY);														} /* AND  HY          */
#define Z80_OPfd_a5 { AND(z80, z80->LY);														} /* AND  LY          */
#define Z80_OPfd_a6 { EAY(z80); AND(z80, RM(z80, z80->ea));									} /* AND  (IY+o)      */
#define Z80_OPfd_a7 { illegal_1(z80); op_a7(z80);												} /* DB   FD          */

#define Z80_OPfd_a8 { illegal_1(z80); op_a8(z80);												} /* DB   FD          */
#define Z80_OPfd_a9 { illegal_1(z80); op_a9(z80);												} /* DB   FD          */
#define Z80_OPfd_aa { illegal_1(z80); op_aa(z80);												} /* DB   FD          */
#define Z80_OPfd_ab { illegal_1(z80); op_ab(z80);												} /* DB   FD          */
#define Z80_OPfd_ac { XOR(z80, z80->HY);														} /* XOR  HY          */
#define Z80_OPfd_ad { XOR(z80, z80->LY);														} /* XOR  LY          */
#define Z80_OPfd_ae { EAY(z80); XOR(z80, RM(z80, z80->ea));									} /* XOR  (IY+o)      */
#define Z80_OPfd_af { illegal_1(z80); op_af(z80);												} /* DB   FD          */

#define Z80_OPfd_b0 { illegal_1(z80); op_b0(z80);												} /* DB   FD          */
#define Z80_OPfd_b1 { illegal_1(z80); op_b1(z80);												} /* DB   FD          */
#define Z80_OPfd_b2 { illegal_1(z80); op_b2(z80);												} /* DB   FD          */
#define Z80_OPfd_b3 { illegal_1(z80); op_b3(z80);												} /* DB   FD          */
#define Z80_OPfd_b4 { OR(z80, z80->HY);														} /* OR   HY          */
#define Z80_OPfd_b5 { OR(z80, z80->LY);														} /* OR   LY          */
#define Z80_OPfd_b6 { EAY(z80); OR(z80, RM(z80, z80->ea));									} /* OR   (IY+o)      */
#define Z80_OPfd_b7 { illegal_1(z80); op_b7(z80);												} /* DB   FD          */

#define Z80_OPfd_b8 { illegal_1(z80); op_b8(z80);												} /* DB   FD          */
#define Z80_OPfd_b9 { illegal_1(z80); op_b9(z80);												} /* DB   FD          */
#define Z80_OPfd_ba { illegal_1(z80); op_ba(z80);												} /* DB   FD          */
#define Z80_OPfd_bb { illegal_1(z80); op_bb(z80);												} /* DB   FD          */
#define Z80_OPfd_bc { CP(z80, z80->HY);														} /* CP   HY          */
#define Z80_OPfd_bd { CP(z80, z80->LY);														} /* CP   LY          */
#define Z80_OPfd_be { EAY(z80); CP(z80, RM(z80, z80->ea));									} /* CP   (IY+o)      */
#define Z80_OPfd_bf { illegal_1(z80); op_bf(z80);												} /* DB   FD          */

#define Z80_OPfd_c0 { illegal_1(z80); op_c0(z80);												} /* DB   FD          */
#define Z80_OPfd_c1 { illegal_1(z80); op_c1(z80);												} /* DB   FD          */
#define Z80_OPfd_c2 { illegal_1(z80); op_c2(z80);												} /* DB   FD          */
#define Z80_OPfd_c3 { illegal_1(z80); op_c3(z80);												} /* DB   FD          */
#define Z80_OPfd_c4 { illegal_1(z80); op_c4(z80);												} /* DB   FD          */
#define Z80_OPfd_c5 { illegal_1(z80); op_c5(z80);												} /* DB   FD          */
#define Z80_OPfd_c6 { illegal_1(z80); op_c6(z80);												} /* DB   FD          */
#define Z80_OPfd_c7 { illegal_1(z80); op_c7(z80);												} /* DB   FD          */

#define Z80_OPfd_c8 { illegal_1(z80); op_c8(z80);												} /* DB   FD          */
#define Z80_OPfd_c9 { illegal_1(z80); op_c9(z80);												} /* DB   FD          */
#define Z80_OPfd_ca { illegal_1(z80); op_ca(z80);												} /* DB   FD          */
#define Z80_OPfd_cb { EAY(z80); EXEC(z80,xycb,ARG(z80));										} /* **   FD CB xx    */
#define Z80_OPfd_cc { illegal_1(z80); op_cc(z80);												} /* DB   FD          */
#define Z80_OPfd_cd { illegal_1(z80); op_cd(z80);												} /* DB   FD          */
#define Z80_OPfd_ce { illegal_1(z80); op_ce(z80);												} /* DB   FD          */
#define Z80_OPfd_cf { illegal_1(z80); op_cf(z80);												} /* DB   FD          */

#define Z80_OPfd_d0 { illegal_1(z80); op_d0(z80);												} /* DB   FD          */
#define Z80_OPfd_d1 { illegal_1(z80); op_d1(z80);												} /* DB   FD          */
#define Z80_OPfd_d2 { illegal_1(z80); op_d2(z80);												} /* DB   FD          */
#define Z80_OPfd_d3 { illegal_1(z80); op_d3(z80);												} /* DB   FD          */
#define Z80_OPfd_d4 { illegal_1(z80); op_d4(z80);												} /* DB   FD          */
#define Z80_OPfd_d5 { illegal_1(z80); op_d5(z80);												} /* DB   FD          */
#define Z80_OPfd_d6 { illegal_1(z80); op_d6(z80);												} /* DB   FD          */
#define Z80_OPfd_d7 { illegal_1(z80); op_d7(z80);												} /* DB   FD          */

#define Z80_OPfd_d8 { illegal_1(z80); op_d8(z80);												} /* DB   FD          */
#define Z80_OPfd_d9 { illegal_1(z80); op_d9(z80);												} /* DB   FD          */
#define Z80_OPfd_da { illegal_1(z80); op_da(z80);												} /* DB   FD          */
#define Z80_OPfd_db { illegal_1(z80); op_db(z80);												} /* DB   FD          */
#define Z80_OPfd_dc { illegal_1(z80); op_dc(z80);												} /* DB   FD          */
#define Z80_OPfd_dd { illegal_1(z80); op_dd(z80);												} /* DB   FD          */
#define Z80_OPfd_de { illegal_1(z80); op_de(z80);												} /* DB   FD          */
#define Z80_OPfd_df { illegal_1(z80); op_df(z80);												} /* DB   FD          */

#define Z80_OPfd_e0 { illegal_1(z80); op_e0(z80);												} /* DB   FD          */
#define Z80_OPfd_e1 { P#define Z80_OPz80, iy);															} /* POP  IY          */
#define Z80_OPfd_e2 { illegal_1(z80); op_e2(z80);												} /* DB   FD          */
#define Z80_OPfd_e3 { EXSP(z80, iy);															} /* EX   (SP),IY     */
#define Z80_OPfd_e4 { illegal_1(z80); op_e4(z80);												} /* DB   FD          */
#define Z80_OPfd_e5 { PUSH(z80, iy);															} /* PUSH IY          */
#define Z80_OPfd_e6 { illegal_1(z80); op_e6(z80);												} /* DB   FD          */
#define Z80_OPfd_e7 { illegal_1(z80); op_e7(z80);												} /* DB   FD          */

#define Z80_OPfd_e8 { illegal_1(z80); op_e8(z80);												} /* DB   FD          */
#define Z80_OPfd_e9 { z80->PC = z80->IY;														} /* JP   (IY)        */
#define Z80_OPfd_ea { illegal_1(z80); op_ea(z80);												} /* DB   FD          */
#define Z80_OPfd_eb { illegal_1(z80); op_eb(z80);												} /* DB   FD          */
#define Z80_OPfd_ec { illegal_1(z80); op_ec(z80);												} /* DB   FD          */
#define Z80_OPfd_ed { illegal_1(z80); op_ed(z80);												} /* DB   FD          */
#define Z80_OPfd_ee { illegal_1(z80); op_ee(z80);												} /* DB   FD          */
#define Z80_OPfd_ef { illegal_1(z80); op_ef(z80);												} /* DB   FD          */

#define Z80_OPfd_f0 { illegal_1(z80); op_f0(z80);												} /* DB   FD          */
#define Z80_OPfd_f1 { illegal_1(z80); op_f1(z80);												} /* DB   FD          */
#define Z80_OPfd_f2 { illegal_1(z80); op_f2(z80);												} /* DB   FD          */
#define Z80_OPfd_f3 { illegal_1(z80); op_f3(z80);												} /* DB   FD          */
#define Z80_OPfd_f4 { illegal_1(z80); op_f4(z80);												} /* DB   FD          */
#define Z80_OPfd_f5 { illegal_1(z80); op_f5(z80);												} /* DB   FD          */
#define Z80_OPfd_f6 { illegal_1(z80); op_f6(z80);												} /* DB   FD          */
#define Z80_OPfd_f7 { illegal_1(z80); op_f7(z80);												} /* DB   FD          */

#define Z80_OPfd_f8 { illegal_1(z80); op_f8(z80);												} /* DB   FD          */
#define Z80_OPfd_f9 { z80->SP = z80->IY;														} /* LD   SP,IY       */
#define Z80_OPfd_fa { illegal_1(z80); op_fa(z80);												} /* DB   FD          */
#define Z80_OPfd_fb { illegal_1(z80); op_fb(z80);												} /* DB   FD          */
#define Z80_OPfd_fc { illegal_1(z80); op_fc(z80);												} /* DB   FD          */
#define Z80_OPfd_fd { illegal_1(z80); op_fd(z80);												} /* DB   FD          */
#define Z80_OPfd_fe { illegal_1(z80); op_fe(z80);												} /* DB   FD          */
#define Z80_OPfd_ff { illegal_1(z80); op_ff(z80);												} /* DB   FD          */

#define Z80_OPillegal,2)
{
	OELog("Z80 '%s' ill. opcode $ed $%02x\n",
		  z80->device->tag, memory_decrypted_read_byte(z80->program, (z80->PCD-1)&0xffff));
}

/**********************************************************
 * special opcodes (ED prefix)
 **********************************************************/
#define Z80_OPed_00 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_01 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_02 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_03 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_04 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_05 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_06 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_07 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_08 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_09 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_0f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_10 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_11 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_12 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_13 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_14 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_15 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_16 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_17 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_18 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_19 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_1f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_20 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_21 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_22 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_23 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_24 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_25 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_26 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_27 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_28 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_29 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_2f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_30 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_31 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_32 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_33 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_34 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_35 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_36 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_37 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_38 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_39 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_3f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_40 { z80->B = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->B];		} /* IN   B,(C)       */
#define Z80_OPed_41 { OUT(z80, z80->BC, z80->B);												} /* OUT  (C),B       */
#define Z80_OPed_42 { SBC16(z80, bc);															} /* SBC  HL,BC       */
#define Z80_OPed_43 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->bc); z80->WZ = z80->ea+1;} /* LD   (w),BC      */
#define Z80_OPed_44 { NEG(z80);																} /* NEG              */
#define Z80_OPed_45 { RETN(z80);																} /* RETN             */
#define Z80_OPed_46 { z80->im = 0;															} /* im   0           */
#define Z80_OPed_47 { LD_I_A(z80);															} /* LD   i,A         */

#define Z80_OPed_48 { z80->C = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->C];		} /* IN   C,(C)       */
#define Z80_OPed_49 { OUT(z80, z80->BC, z80->C);												} /* OUT  (C),C       */
#define Z80_OPed_4a { ADC16(z80, bc);															} /* ADC  HL,BC       */
#define Z80_OPed_4b { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->bc); z80->WZ = z80->ea+1;} /* LD   BC,(w)      */
#define Z80_OPed_4c { NEG(z80);																} /* NEG              */
#define Z80_OPed_4d { RETI(z80);																} /* RETI             */
#define Z80_OPed_4e { z80->im = 0;															} /* im   0           */
#define Z80_OPed_4f { LD_R_A(z80);															} /* LD   r,A         */

#define Z80_OPed_50 { z80->D = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->D];		} /* IN   D,(C)       */
#define Z80_OPed_51 { OUT(z80, z80->BC, z80->D);												} /* OUT  (C),D       */
#define Z80_OPed_52 { SBC16(z80, de);															} /* SBC  HL,DE       */
#define Z80_OPed_53 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->de); z80->WZ = z80->ea+1;} /* LD   (w),DE      */
#define Z80_OPed_54 { NEG(z80);																} /* NEG              */
#define Z80_OPed_55 { RETN(z80);																} /* RETN             */
#define Z80_OPed_56 { z80->im = 1;															} /* im   1           */
#define Z80_OPed_57 { LD_A_I(z80);															} /* LD   A,i         */

#define Z80_OPed_58 { z80->E = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->E];		} /* IN   E,(C)       */
#define Z80_OPed_59 { OUT(z80, z80->BC, z80->E);												} /* OUT  (C),E       */
#define Z80_OPed_5a { ADC16(z80, de);															} /* ADC  HL,DE       */
#define Z80_OPed_5b { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->de); z80->WZ = z80->ea+1;} /* LD   DE,(w)      */
#define Z80_OPed_5c { NEG(z80);																} /* NEG              */
#define Z80_OPed_5d { RETI(z80);																} /* RETI             */
#define Z80_OPed_5e { z80->im = 2;															} /* im   2           */
#define Z80_OPed_5f { LD_A_R(z80);															} /* LD   A,r         */

#define Z80_OPed_60 { z80->H = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->H];		} /* IN   H,(C)       */
#define Z80_OPed_61 { OUT(z80, z80->BC, z80->H);												} /* OUT  (C),H       */
#define Z80_OPed_62 { SBC16(z80, hl);															} /* SBC  HL,HL       */
#define Z80_OPed_63 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->hl); z80->WZ = z80->ea+1;} /* LD   (w),HL      */
#define Z80_OPed_64 { NEG(z80);																} /* NEG              */
#define Z80_OPed_65 { RETN(z80);																} /* RETN             */
#define Z80_OPed_66 { z80->im = 0;															} /* im   0           */
#define Z80_OPed_67 { RRD(z80);																} /* RRD  (HL)        */

#define Z80_OPed_68 { z80->L = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->L];		} /* IN   L,(C)       */
#define Z80_OPed_69 { OUT(z80, z80->BC, z80->L);												} /* OUT  (C),L       */
#define Z80_OPed_6a { ADC16(z80, hl);															} /* ADC  HL,HL       */
#define Z80_OPed_6b { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->hl); z80->WZ = z80->ea+1;} /* LD   HL,(w)      */
#define Z80_OPed_6c { NEG(z80);																} /* NEG              */
#define Z80_OPed_6d { RETI(z80);																} /* RETI             */
#define Z80_OPed_6e { z80->im = 0;															} /* im   0           */
#define Z80_OPed_6f { RLD(z80);																} /* RLD  (HL)        */

#define Z80_OPed_70 { UINT8 res = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[res];		} /* IN   0,(C)       */
#define Z80_OPed_71 { OUT(z80, z80->BC, 0);													} /* OUT  (C),0       */
#define Z80_OPed_72 { SBC16(z80, sp);															} /* SBC  HL,SP       */
#define Z80_OPed_73 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->sp); z80->WZ = z80->ea+1;} /* LD   (w),SP      */
#define Z80_OPed_74 { NEG(z80);																} /* NEG              */
#define Z80_OPed_75 { RETN(z80);																} /* RETN             */
#define Z80_OPed_76 { z80->im = 1;															} /* im   1           */
#define Z80_OPed_77 { illegal_2(z80);															} /* DB   ED,77       */

#define Z80_OPed_78 { z80->A = IN(z80, z80->BC); z80->F = (z80->F & CF) | SZP[z80->A]; z80->WZ = z80->BC + 1;	} /* IN   A,(C)       */
#define Z80_OPed_79 { OUT(z80, z80->BC, z80->A);	z80->WZ = z80->BC + 1;						} /* OUT  (C),A       */
#define Z80_OPed_7a { ADC16(z80, sp);															} /* ADC  HL,SP       */
#define Z80_OPed_7b { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->sp); z80->WZ = z80->ea+1;} /* LD   SP,(w)      */
#define Z80_OPed_7c { NEG(z80);																} /* NEG              */
#define Z80_OPed_7d { RETI(z80);																} /* RETI             */
#define Z80_OPed_7e { z80->im = 2;															} /* im   2           */
#define Z80_OPed_7f { illegal_2(z80);															} /* DB   ED,7F       */

#define Z80_OPed_80 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_81 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_82 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_83 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_84 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_85 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_86 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_87 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_88 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_89 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_8f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_90 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_91 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_92 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_93 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_94 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_95 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_96 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_97 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_98 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_99 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9a { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9b { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9c { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9d { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9e { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_9f { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_a0 { LDI(z80);																} /* LDI              */
#define Z80_OPed_a1 { CPI(z80);																} /* CPI              */
#define Z80_OPed_a2 { INI(z80);																} /* INI              */
#define Z80_OPed_a3 { OUTI(z80);																} /* OUTI             */
#define Z80_OPed_a4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_a5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_a6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_a7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_a8 { LDD(z80);																} /* LDD              */
#define Z80_OPed_a9 { CPD(z80);																} /* CPD              */
#define Z80_OPed_aa { IND(z80);																} /* IND              */
#define Z80_OPed_ab { OUTD(z80);																} /* OUTD             */
#define Z80_OPed_ac { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ad { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ae { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_af { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_b0 { LDIR(z80);																} /* LDIR             */
#define Z80_OPed_b1 { CPIR(z80);																} /* CPIR             */
#define Z80_OPed_b2 { INIR(z80);																} /* INIR             */
#define Z80_OPed_b3 { OTIR(z80);																} /* OTIR             */
#define Z80_OPed_b4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_b5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_b6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_b7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_b8 { LDDR(z80);																} /* LDDR             */
#define Z80_OPed_b9 { CPDR(z80);																} /* CPDR             */
#define Z80_OPed_ba { INDR(z80);																} /* INDR             */
#define Z80_OPed_bb { OTDR(z80);																} /* OTDR             */
#define Z80_OPed_bc { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_bd { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_be { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_bf { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_c0 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c1 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c2 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c3 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_c8 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_c9 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ca { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_cb { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_cc { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_cd { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ce { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_cf { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_d0 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d1 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d2 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d3 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_d8 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_d9 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_da { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_db { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_dc { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_dd { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_de { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_df { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_e0 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e1 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e2 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e3 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_e8 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_e9 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ea { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_eb { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ec { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ed { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ee { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ef { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_f0 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f1 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f2 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f3 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f4 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f5 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f6 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f7 { illegal_2(z80);															} /* DB   ED          */

#define Z80_OPed_f8 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_f9 { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_fa { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_fb { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_fc { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_fd { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_fe { illegal_2(z80);															} /* DB   ED          */
#define Z80_OPed_ff { illegal_2(z80);															} /* DB   ED          */


/**********************************************************
 * main opcodes
 **********************************************************/
#define Z80_OP00 {																			} /* NOP              */
#define Z80_OP01 { z80->BC = ARG16(z80);													} /* LD   BC,w        */
#define Z80_OP02 { WM(z80,z80->BC,z80->A); z80->WZ_L = (z80->BC + 1) & 0xFF;  z80->WZ_H = z80->A; } /* LD (BC),A */
#define Z80_OP03 { z80->BC++;																} /* INC  BC          */
#define Z80_OP04 { z80->B = INC(z80, z80->B);												} /* INC  B           */
#define Z80_OP05 { z80->B = DEC(z80, z80->B);												} /* DEC  B           */
#define Z80_OP06 { z80->B = ARG(z80);														} /* LD   B,n         */
#define Z80_OP07 { RLCA(z80);																} /* RLCA             */

#define Z80_OP08 { EX_AF(z80);																} /* EX   AF,AF'      */
#define Z80_OP09 { ADD16(z80, hl, bc);														} /* ADD  HL,BC       */
#define Z80_OP0a { z80->A = RM(z80, z80->BC);	z80->WZ=z80->BC+1;  						} /* LD   A,(BC)      */
#define Z80_OP0b { z80->BC--;																} /* DEC  BC          */
#define Z80_OP0c { z80->C = INC(z80, z80->C);												} /* INC  C           */
#define Z80_OP0d { z80->C = DEC(z80, z80->C);												} /* DEC  C           */
#define Z80_OP0e { z80->C = ARG(z80);														} /* LD   C,n         */
#define Z80_OP0f { RRCA(z80);																} /* RRCA             */

#define Z80_OP10 { z80->B--; JR_COND(z80, z80->B, 0x10);									} /* DJNZ o           */
#define Z80_OP11 { z80->DE = ARG16(z80);													} /* LD   DE,w        */
#define Z80_OP12 { WM(z80,z80->DE,z80->A); z80->WZ_L = (z80->DE + 1) & 0xFF;  z80->WZ_H = z80->A; } /* LD (DE),A */
#define Z80_OP13 { z80->DE++;																} /* INC  DE          */
#define Z80_OP14 { z80->D = INC(z80, z80->D);												} /* INC  D           */
#define Z80_OP15 { z80->D = DEC(z80, z80->D);												} /* DEC  D           */
#define Z80_OP16 { z80->D = ARG(z80);														} /* LD   D,n         */
#define Z80_OP17 { RLA(z80);																} /* RLA              */

#define Z80_OP18 { JR(z80);																} /* JR   o           */
#define Z80_OP19 { ADD16(z80, hl, de);														} /* ADD  HL,DE       */
#define Z80_OP1a { z80->A = RM(z80, z80->DE); z80->WZ=z80->DE+1;							} /* LD   A,(DE)      */
#define Z80_OP1b { z80->DE--;																} /* DEC  DE          */
#define Z80_OP1c { z80->E = INC(z80, z80->E);												} /* INC  E           */
#define Z80_OP1d { z80->E = DEC(z80, z80->E);												} /* DEC  E           */
#define Z80_OP1e { z80->E = ARG(z80);														} /* LD   E,n         */
#define Z80_OP1f { RRA(z80);																} /* RRA              */

#define Z80_OP20 { JR_COND(z80, !(z80->F & ZF), 0x20);										} /* JR   NZ,o        */
#define Z80_OP21 { z80->HL = ARG16(z80);													} /* LD   HL,w        */
#define Z80_OP22 { z80->ea = ARG16(z80); WM16(z80, z80->ea, &z80->hl);	z80->WZ = z80->ea+1;} /* LD   (w),HL      */
#define Z80_OP23 { z80->HL++;																} /* INC  HL          */
#define Z80_OP24 { z80->H = INC(z80, z80->H);												} /* INC  H           */
#define Z80_OP25 { z80->H = DEC(z80, z80->H);												} /* DEC  H           */
#define Z80_OP26 { z80->H = ARG(z80);														} /* LD   H,n         */
#define Z80_OP27 { DAA(z80);																} /* DAA              */

#define Z80_OP28 { JR_COND(z80, z80->F & ZF, 0x28);										} /* JR   Z,o         */
#define Z80_OP29 { ADD16(z80, hl, hl);														} /* ADD  HL,HL       */
#define Z80_OP2a { z80->ea = ARG16(z80); RM16(z80, z80->ea, &z80->hl);	z80->WZ = z80->ea+1;} /* LD   HL,(w)      */
#define Z80_OP2b { z80->HL--;																} /* DEC  HL          */
#define Z80_OP2c { z80->L = INC(z80, z80->L);												} /* INC  L           */
#define Z80_OP2d { z80->L = DEC(z80, z80->L);												} /* DEC  L           */
#define Z80_OP2e { z80->L = ARG(z80);														} /* LD   L,n         */
#define Z80_OP2f { z80->A ^= 0xff; z80->F = (z80->F&(SF|ZF|PF|CF))|HF|NF|(z80->A&(YF|XF));	} /* CPL              */

#define Z80_OP30 { JR_COND(z80, !(z80->F & CF), 0x30);										} /* JR   NC,o        */
#define Z80_OP31 { z80->SP = ARG16(z80);													} /* LD   SP,w        */
#define Z80_OP32 { z80->ea=ARG16(z80);WM(z80,z80->ea,z80->A);z80->WZ_L=(z80->ea+1)&0xFF;z80->WZ_H=z80->A; } /* LD   (w),A       */
#define Z80_OP33 { z80->SP++;																} /* INC  SP          */
#define Z80_OP34 { WM(z80, z80->HL, INC(z80, RM(z80, z80->HL)));							} /* INC  (HL)        */
#define Z80_OP35 { WM(z80, z80->HL, DEC(z80, RM(z80, z80->HL)));							} /* DEC  (HL)        */
#define Z80_OP36 { WM(z80, z80->HL, ARG(z80));												} /* LD   (HL),n      */
#define Z80_OP37 { z80->F = (z80->F & (SF|ZF|YF|XF|PF)) | CF | (z80->A & (YF|XF));			} /* SCF              */

#define Z80_OP38 { JR_COND(z80, z80->F & CF, 0x38);										} /* JR   C,o         */
#define Z80_OP39 { ADD16(z80, hl, sp);														} /* ADD  HL,SP       */
#define Z80_OP3a { z80->ea = ARG16(z80); z80->A = RM(z80, z80->ea); z80->WZ=z80->ea+1;		} /* LD   A,(w)       */
#define Z80_OP3b { z80->SP--;																} /* DEC  SP          */
#define Z80_OP3c { z80->A = INC(z80, z80->A);												} /* INC  A           */
#define Z80_OP3d { z80->A = DEC(z80, z80->A);												} /* DEC  A           */
#define Z80_OP3e { z80->A = ARG(z80);														} /* LD   A,n         */
#define Z80_OP3f { z80->F = ((z80->F&(SF|ZF|YF|XF|PF|CF))|((z80->F&CF)<<4)|(z80->A&(YF|XF)))^CF; } /* CCF        */

#define Z80_OP40 {																			} /* LD   B,B         */
#define Z80_OP41 { z80->B = z80->C;														} /* LD   B,C         */
#define Z80_OP42 { z80->B = z80->D;														} /* LD   B,D         */
#define Z80_OP43 { z80->B = z80->E;														} /* LD   B,E         */
#define Z80_OP44 { z80->B = z80->H;														} /* LD   B,H         */
#define Z80_OP45 { z80->B = z80->L;														} /* LD   B,L         */
#define Z80_OP46 { z80->B = RM(z80, z80->HL);												} /* LD   B,(HL)      */
#define Z80_OP47 { z80->B = z80->A;														} /* LD   B,A         */

#define Z80_OP48 { z80->C = z80->B;														} /* LD   C,B         */
#define Z80_OP49 {																			} /* LD   C,C         */
#define Z80_OP4a { z80->C = z80->D;														} /* LD   C,D         */
#define Z80_OP4b { z80->C = z80->E;														} /* LD   C,E         */
#define Z80_OP4c { z80->C = z80->H;														} /* LD   C,H         */
#define Z80_OP4d { z80->C = z80->L;														} /* LD   C,L         */
#define Z80_OP4e { z80->C = RM(z80, z80->HL);												} /* LD   C,(HL)      */
#define Z80_OP4f { z80->C = z80->A;														} /* LD   C,A         */

#define Z80_OP50 { z80->D = z80->B;														} /* LD   D,B         */
#define Z80_OP51 { z80->D = z80->C;														} /* LD   D,C         */
#define Z80_OP52 {																			} /* LD   D,D         */
#define Z80_OP53 { z80->D = z80->E;														} /* LD   D,E         */
#define Z80_OP54 { z80->D = z80->H;														} /* LD   D,H         */
#define Z80_OP55 { z80->D = z80->L;														} /* LD   D,L         */
#define Z80_OP56 { z80->D = RM(z80, z80->HL);												} /* LD   D,(HL)      */
#define Z80_OP57 { z80->D = z80->A;														} /* LD   D,A         */

#define Z80_OP58 { z80->E = z80->B;														} /* LD   E,B         */
#define Z80_OP59 { z80->E = z80->C;														} /* LD   E,C         */
#define Z80_OP5a { z80->E = z80->D;														} /* LD   E,D         */
#define Z80_OP5b {																			} /* LD   E,E         */
#define Z80_OP5c { z80->E = z80->H;														} /* LD   E,H         */
#define Z80_OP5d { z80->E = z80->L;														} /* LD   E,L         */
#define Z80_OP5e { z80->E = RM(z80, z80->HL);												} /* LD   E,(HL)      */
#define Z80_OP5f { z80->E = z80->A;														} /* LD   E,A         */

#define Z80_OP60 { z80->H = z80->B;														} /* LD   H,B         */
#define Z80_OP61 { z80->H = z80->C;														} /* LD   H,C         */
#define Z80_OP62 { z80->H = z80->D;														} /* LD   H,D         */
#define Z80_OP63 { z80->H = z80->E;														} /* LD   H,E         */
#define Z80_OP64 {																			} /* LD   H,H         */
#define Z80_OP65 { z80->H = z80->L;														} /* LD   H,L         */
#define Z80_OP66 { z80->H = RM(z80, z80->HL);												} /* LD   H,(HL)      */
#define Z80_OP67 { z80->H = z80->A;														} /* LD   H,A         */

#define Z80_OP68 { z80->L = z80->B;														} /* LD   L,B         */
#define Z80_OP69 { z80->L = z80->C;														} /* LD   L,C         */
#define Z80_OP6a { z80->L = z80->D;														} /* LD   L,D         */
#define Z80_OP6b { z80->L = z80->E;														} /* LD   L,E         */
#define Z80_OP6c { z80->L = z80->H;														} /* LD   L,H         */
#define Z80_OP6d {																			} /* LD   L,L         */
#define Z80_OP6e { z80->L = RM(z80, z80->HL);												} /* LD   L,(HL)      */
#define Z80_OP6f { z80->L = z80->A;														} /* LD   L,A         */

#define Z80_OP70 { WM(z80, z80->HL, z80->B);												} /* LD   (HL),B      */
#define Z80_OP71 { WM(z80, z80->HL, z80->C);												} /* LD   (HL),C      */
#define Z80_OP72 { WM(z80, z80->HL, z80->D);												} /* LD   (HL),D      */
#define Z80_OP73 { WM(z80, z80->HL, z80->E);												} /* LD   (HL),E      */
#define Z80_OP74 { WM(z80, z80->HL, z80->H);												} /* LD   (HL),H      */
#define Z80_OP75 { WM(z80, z80->HL, z80->L);												} /* LD   (HL),L      */
#define Z80_OP76 { ENTER_HALT(z80);														} /* halt             */
#define Z80_OP77 { WM(z80, z80->HL, z80->A);												} /* LD   (HL),A      */

#define Z80_OP78 { z80->A = z80->B;														} /* LD   A,B         */
#define Z80_OP79 { z80->A = z80->C;														} /* LD   A,C         */
#define Z80_OP7a { z80->A = z80->D;														} /* LD   A,D         */
#define Z80_OP7b { z80->A = z80->E;														} /* LD   A,E         */
#define Z80_OP7c { z80->A = z80->H;														} /* LD   A,H         */
#define Z80_OP7d { z80->A = z80->L;														} /* LD   A,L         */
#define Z80_OP7e { z80->A = RM(z80, z80->HL);												} /* LD   A,(HL)      */
#define Z80_OP7f {																			} /* LD   A,A         */

#define Z80_OP80 { ADD(z80, z80->B);														} /* ADD  A,B         */
#define Z80_OP81 { ADD(z80, z80->C);														} /* ADD  A,C         */
#define Z80_OP82 { ADD(z80, z80->D);														} /* ADD  A,D         */
#define Z80_OP83 { ADD(z80, z80->E);														} /* ADD  A,E         */
#define Z80_OP84 { ADD(z80, z80->H);														} /* ADD  A,H         */
#define Z80_OP85 { ADD(z80, z80->L);														} /* ADD  A,L         */
#define Z80_OP86 { ADD(z80, RM(z80, z80->HL));												} /* ADD  A,(HL)      */
#define Z80_OP87 { ADD(z80, z80->A);														} /* ADD  A,A         */

#define Z80_OP88 { ADC(z80, z80->B);														} /* ADC  A,B         */
#define Z80_OP89 { ADC(z80, z80->C);														} /* ADC  A,C         */
#define Z80_OP8a { ADC(z80, z80->D);														} /* ADC  A,D         */
#define Z80_OP8b { ADC(z80, z80->E);														} /* ADC  A,E         */
#define Z80_OP8c { ADC(z80, z80->H);														} /* ADC  A,H         */
#define Z80_OP8d { ADC(z80, z80->L);														} /* ADC  A,L         */
#define Z80_OP8e { ADC(z80, RM(z80, z80->HL));												} /* ADC  A,(HL)      */
#define Z80_OP8f { ADC(z80, z80->A);														} /* ADC  A,A         */

#define Z80_OP90 { SUB(z80, z80->B);														} /* SUB  B           */
#define Z80_OP91 { SUB(z80, z80->C);														} /* SUB  C           */
#define Z80_OP92 { SUB(z80, z80->D);														} /* SUB  D           */
#define Z80_OP93 { SUB(z80, z80->E);														} /* SUB  E           */
#define Z80_OP94 { SUB(z80, z80->H);														} /* SUB  H           */
#define Z80_OP95 { SUB(z80, z80->L);														} /* SUB  L           */
#define Z80_OP96 { SUB(z80, RM(z80, z80->HL));												} /* SUB  (HL)        */
#define Z80_OP97 { SUB(z80, z80->A);														} /* SUB  A           */

#define Z80_OP98 { SBC(z80, z80->B);														} /* SBC  A,B         */
#define Z80_OP99 { SBC(z80, z80->C);														} /* SBC  A,C         */
#define Z80_OP9a { SBC(z80, z80->D);														} /* SBC  A,D         */
#define Z80_OP9b { SBC(z80, z80->E);														} /* SBC  A,E         */
#define Z80_OP9c { SBC(z80, z80->H);														} /* SBC  A,H         */
#define Z80_OP9d { SBC(z80, z80->L);														} /* SBC  A,L         */
#define Z80_OP9e { SBC(z80, RM(z80, z80->HL));												} /* SBC  A,(HL)      */
#define Z80_OP9f { SBC(z80, z80->A);														} /* SBC  A,A         */

#define Z80_OPa0 { AND(z80, z80->B);														} /* AND  B           */
#define Z80_OPa1 { AND(z80, z80->C);														} /* AND  C           */
#define Z80_OPa2 { AND(z80, z80->D);														} /* AND  D           */
#define Z80_OPa3 { AND(z80, z80->E);														} /* AND  E           */
#define Z80_OPa4 { AND(z80, z80->H);														} /* AND  H           */
#define Z80_OPa5 { AND(z80, z80->L);														} /* AND  L           */
#define Z80_OPa6 { AND(z80, RM(z80, z80->HL));												} /* AND  (HL)        */
#define Z80_OPa7 { AND(z80, z80->A);														} /* AND  A           */

#define Z80_OPa8 { XOR(z80, z80->B);														} /* XOR  B           */
#define Z80_OPa9 { XOR(z80, z80->C);														} /* XOR  C           */
#define Z80_OPaa { XOR(z80, z80->D);														} /* XOR  D           */
#define Z80_OPab { XOR(z80, z80->E);														} /* XOR  E           */
#define Z80_OPac { XOR(z80, z80->H);														} /* XOR  H           */
#define Z80_OPad { XOR(z80, z80->L);														} /* XOR  L           */
#define Z80_OPae { XOR(z80, RM(z80, z80->HL));												} /* XOR  (HL)        */
#define Z80_OPaf { XOR(z80, z80->A);														} /* XOR  A           */

#define Z80_OPb0 { OR(z80, z80->B);														} /* OR   B           */
#define Z80_OPb1 { OR(z80, z80->C);														} /* OR   C           */
#define Z80_OPb2 { OR(z80, z80->D);														} /* OR   D           */
#define Z80_OPb3 { OR(z80, z80->E);														} /* OR   E           */
#define Z80_OPb4 { OR(z80, z80->H);														} /* OR   H           */
#define Z80_OPb5 { OR(z80, z80->L);														} /* OR   L           */
#define Z80_OPb6 { OR(z80, RM(z80, z80->HL));												} /* OR   (HL)        */
#define Z80_OPb7 { OR(z80, z80->A);														} /* OR   A           */

#define Z80_OPb8 { CP(z80, z80->B);														} /* CP   B           */
#define Z80_OPb9 { CP(z80, z80->C);														} /* CP   C           */
#define Z80_OPba { CP(z80, z80->D);														} /* CP   D           */
#define Z80_OPbb { CP(z80, z80->E);														} /* CP   E           */
#define Z80_OPbc { CP(z80, z80->H);														} /* CP   H           */
#define Z80_OPbd { CP(z80, z80->L);														} /* CP   L           */
#define Z80_OPbe { CP(z80, RM(z80, z80->HL));												} /* CP   (HL)        */
#define Z80_OPbf { CP(z80, z80->A);														} /* CP   A           */

#define Z80_OPc0 { RET_COND(z80, !(z80->F & ZF), 0xc0);									} /* RET  NZ          */
#define Z80_OPc1 { P#define Z80_OPz80, bc);															} /* POP  BC          */
#define Z80_OPc2 { JP_COND(z80, !(z80->F & ZF));											} /* JP   NZ,a        */
#define Z80_OPc3 { JP(z80);																} /* JP   a           */
#define Z80_OPc4 { CALL_COND(z80, !(z80->F & ZF), 0xc4);									} /* CALL NZ,a        */
#define Z80_OPc5 { PUSH(z80, bc);															} /* PUSH BC          */
#define Z80_OPc6 { ADD(z80, ARG(z80));														} /* ADD  A,n         */
#define Z80_OPc7 { RST(z80, 0x00);															} /* RST  0           */

#define Z80_OPc8 { RET_COND(z80, z80->F & ZF, 0xc8);										} /* RET  Z           */
#define Z80_OPc9 { P#define Z80_OPz80, pc); z80->WZ=z80->PCD;											} /* RET              */
#define Z80_OPca { JP_COND(z80, z80->F & ZF);												} /* JP   Z,a         */
#define Z80_OPcb { z80->r++; EXEC(z80,cb,R#define Z80_OPz80));										} /* **** CB xx       */
#define Z80_OPcc { CALL_COND(z80, z80->F & ZF, 0xcc);										} /* CALL Z,a         */
#define Z80_OPcd { CALL(z80);																} /* CALL a           */
#define Z80_OPce { ADC(z80, ARG(z80));														} /* ADC  A,n         */
#define Z80_OPcf { RST(z80, 0x08);															} /* RST  1           */

#define Z80_OPd0 { RET_COND(z80, !(z80->F & CF), 0xd0);									} /* RET  NC          */
#define Z80_OPd1 { P#define Z80_OPz80, de);															} /* POP  DE          */
#define Z80_OPd2 { JP_COND(z80, !(z80->F & CF));											} /* JP   NC,a        */
#define Z80_OPd3 { unsigned n = ARG(z80) | (z80->A << 8); OUT(z80, n, z80->A);	z80->WZ_L = ((n & 0xff) + 1) & 0xff;  z80->WZ_H = z80->A;	} /* OUT  (n),A       */
#define Z80_OPd4 { CALL_COND(z80, !(z80->F & CF), 0xd4);									} /* CALL NC,a        */
#define Z80_OPd5 { PUSH(z80, de);															} /* PUSH DE          */
#define Z80_OPd6 { SUB(z80, ARG(z80));														} /* SUB  n           */
#define Z80_OPd7 { RST(z80, 0x10);															} /* RST  2           */

#define Z80_OPd8 { RET_COND(z80, z80->F & CF, 0xd8);										} /* RET  C           */
#define Z80_OPd9 { EXX(z80);																} /* EXX              */
#define Z80_OPda { JP_COND(z80, z80->F & CF);												} /* JP   C,a         */
#define Z80_OPdb { unsigned n = ARG(z80) | (z80->A << 8); z80->A = IN(z80, n);	z80->WZ = n + 1; } /* IN   A,(n)  */
#define Z80_OPdc { CALL_COND(z80, z80->F & CF, 0xdc);										} /* CALL C,a         */
#define Z80_OPdd { z80->r++; EXEC(z80,dd,R#define Z80_OPz80));										} /* **** DD xx       */
#define Z80_OPde { SBC(z80, ARG(z80));														} /* SBC  A,n         */
#define Z80_OPdf { RST(z80, 0x18);															} /* RST  3           */

#define Z80_OPe0 { RET_COND(z80, !(z80->F & PF), 0xe0);									} /* RET  PO          */
#define Z80_OPe1 { P#define Z80_OPz80, hl);															} /* POP  HL          */
#define Z80_OPe2 { JP_COND(z80, !(z80->F & PF));											} /* JP   PO,a        */
#define Z80_OPe3 { EXSP(z80, hl);															} /* EX   HL,(SP)     */
#define Z80_OPe4 { CALL_COND(z80, !(z80->F & PF), 0xe4);									} /* CALL PO,a        */
#define Z80_OPe5 { PUSH(z80, hl);															} /* PUSH HL          */
#define Z80_OPe6 { AND(z80, ARG(z80));														} /* AND  n           */
#define Z80_OPe7 { RST(z80, 0x20);															} /* RST  4           */

#define Z80_OPe8 { RET_COND(z80, z80->F & PF, 0xe8);										} /* RET  PE          */
#define Z80_OPe9 { z80->PC = z80->HL;														} /* JP   (HL)        */
#define Z80_OPea { JP_COND(z80, z80->F & PF);												} /* JP   PE,a        */
#define Z80_OPeb { EX_DE_HL(z80);															} /* EX   DE,HL       */
#define Z80_OPec { CALL_COND(z80, z80->F & PF, 0xec);										} /* CALL PE,a        */
#define Z80_OPed { z80->r++; EXEC(z80,ed,R#define Z80_OPz80));										} /* **** ED xx       */
#define Z80_OPee { XOR(z80, ARG(z80));														} /* XOR  n           */
#define Z80_OPef { RST(z80, 0x28);															} /* RST  5           */

#define Z80_OPf0 { RET_COND(z80, !(z80->F & SF), 0xf0);									} /* RET  P           */
#define Z80_OPf1 { P#define Z80_OPz80, af);															} /* POP  AF          */
#define Z80_OPf2 { JP_COND(z80, !(z80->F & SF));											} /* JP   P,a         */
#define Z80_OPf3 { z80->iff1 = z80->iff2 = 0;												} /* DI               */
#define Z80_OPf4 { CALL_COND(z80, !(z80->F & SF), 0xf4);									} /* CALL P,a         */
#define Z80_OPf5 { PUSH(z80, af);															} /* PUSH AF          */
#define Z80_OPf6 { OR(z80, ARG(z80));														} /* OR   n           */
#define Z80_OPf7 { RST(z80, 0x30);															} /* RST  6           */

#define Z80_OPf8 { RET_COND(z80, z80->F & SF, 0xf8);										} /* RET  M           */
#define Z80_OPf9 { z80->SP = z80->HL;														} /* LD   SP,HL       */
#define Z80_OPfa { JP_COND(z80, z80->F & SF);												} /* JP   M,a         */
#define Z80_OPfb { EI(z80);																} /* EI               */
#define Z80_OPfc { CALL_COND(z80, z80->F & SF, 0xfc);										} /* CALL M,a         */
#define Z80_OPfd { z80->r++; EXEC(z80,fd,R#define Z80_OPz80));										} /* **** FD xx       */
#define Z80_OPfe { CP(z80, ARG(z80));														} /* CP   n           */
#define Z80_OPff { RST(z80, 0x38);															} /* RST  7           */
