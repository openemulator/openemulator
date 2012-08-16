
/**
 * libemulation
 * Apple III MOS6502 Operations
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements AppleIIIMOS6502 operations
 */

#define APPLEIIIRDMEM_ID(a) extendedMemoryBus->read(a); icount--
#define APPLEIIIWRMEM_ID(a,d) extendedMemoryBus->write(a, d); icount--

#define APPLEIIIRD_IDY_P                                    \
EA_IDY_P;                                                   \
if (extendedMemoryEnabled)                                  \
{                                                           \
    int xbyte = memoryBus->read(extendedPageAddress | ZPA); \
    if (xbyte & 0x80)                                       \
    {                                                       \
        xbyte &= 0x0f;                                      \
        systemControl->postMessage(this,                    \
            APPLEIII_SET_EXTENDEDRAMBANK, &xbyte);          \
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
    int xbyte = memoryBus->read(extendedPageAddress | ZPA); \
    if (xbyte & 0x80)                                       \
    {                                                       \
        xbyte &= 0x0f;                                      \
        systemControl->postMessage(this,                    \
            APPLEIII_SET_EXTENDEDRAMBANK, &xbyte);          \
        APPLEIIIWRMEM_ID(EAA, tmp);                         \
    }                                                       \
    else                                                    \
        WRMEM_ID(EAA, tmp);                                 \
}                                                           \
else                                                        \
    WRMEM_ID(EAA, tmp)
