
/**
 * libemulation
 * W65C02S
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a W65C02S microprocessor
 */

#include "MOS6502.h"

class W65C02S : public MOS6502
{
private:
    void execute();
};
