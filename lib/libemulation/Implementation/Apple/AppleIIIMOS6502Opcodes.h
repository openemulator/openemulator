
/**
 * libemulation
 * Apple III MOS6502 Opcodes
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III MOS6502 opcodes
 */

#include "MOS6502Opcodes.h"
#include "AppleIIIMOS6502Operations.h"

#define APPLEIIIMOS6502_OP(nn) case 0x##nn: APPLEIIIMOS6502_OP##nn; break

#define APPLEIIIMOS6502_OP11 { int tmp; APPLEIIIRD_IDY_P; ORA;		} /* 5 ORA IDY page penalty */
#define APPLEIIIMOS6502_OP31 { int tmp; APPLEIIIRD_IDY_P; AND;		} /* 5 AND IDY page penalty */
#define APPLEIIIMOS6502_OP51 { int tmp; APPLEIIIRD_IDY_P; EOR;		} /* 5 EOR IDY page penalty */
#define APPLEIIIMOS6502_OP71 { int tmp; APPLEIIIRD_IDY_P; ADC;		} /* 5 ADC IDY page penalty */
#define APPLEIIIMOS6502_OP91 { int tmp; STA; APPLEIIIWR_IDY_NP;		} /* 6 STA IDY */
#define APPLEIIIMOS6502_OPb1 { int tmp; APPLEIIIRD_IDY_P; LDA;		} /* 5 LDA IDY page penalty */
#define APPLEIIIMOS6502_OPd1 { int tmp; APPLEIIIRD_IDY_P; CMP;		} /* 5 CMP IDY page penalty */
#define APPLEIIIMOS6502_OPf1 { int tmp; APPLEIIIRD_IDY_P; SBC;		} /* 5 SBC IDY page penalty */
