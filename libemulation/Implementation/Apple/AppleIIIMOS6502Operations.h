
/**
 * libemulation
 * Apple III MOS6502 Operations
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements AppleIIIMOS6502 operations
 */


#ifndef OpenEmulator_AppleIIIMOS6502Operations_h
#define OpenEmulator_AppleIIIMOS6502Operations_h

#define APPLEIIIRDMEM_ID(a) extendedMemoryBus->read(a); icount--
#define APPLEIIIWRMEM_ID(a,d) extendedMemoryBus->write(a, d); icount--

#define APPLEIIIRD_IDY_P                                    \
EA_IDY_P;                                                   \
if (extendedMemoryEnabled)                                  \
{                                                           \
    int xbyte = memoryBus->read(0xc00 ^ ZPA);               \
    if (xbyte & 0x80)                                       \
    {                                                       \
        setExtendedMemoryBank(xbyte & 0xf);                 \
        tmp = APPLEIIIRDMEM_ID(EAA);                        \
    }                                                       \
    else                                                    \
        tmp = RDMEM_ID(EAA);                                \
}                                                           \
else                                                        \
    tmp = RDMEM_ID(EAA)

#define APPLEIIIWR_IDY_NP                                   \
EA_IDY_NP;                                                  \
if (extendedMemoryEnabled)                                  \
{                                                           \
    int xbyte = memoryBus->read(0xc00 ^ ZPA);               \
    if (xbyte & 0x80)                                       \
    {                                                       \
        setExtendedMemoryBank(xbyte & 0xf);                 \
        APPLEIIIWRMEM_ID(EAA, tmp);                         \
    }                                                       \
    else                                                    \
        WRMEM_ID(EAA, tmp);                                 \
}                                                           \
else                                                        \
    WRMEM_ID(EAA, tmp)

#endif
