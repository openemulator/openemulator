
/**
 * libemulation
 * Apple III MOS6502 Operations
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements AppleIIIMOS6502 operations
 */

#define APPLEIIIRDMEM(a) extendedMemoryBus->read(a); icount--
#define APPLEIIIWRMEM(a,d) extendedMemoryBus->write(a, d); icount--

#define APPLEIIIRDMEM_ID(a) extendedMemoryBus->read(a); icount--
#define APPLEIIIWRMEM_ID(a,d) extendedMemoryBus->write(a, d); icount--

#define APPLEIIIRD_IDY_P                                    \
ZPL = RDOPARG();											\
EAL = RDMEM(ZPA);											\
ZPL++;														\
EAH = RDMEM(ZPA);											\
if (extendedMemoryEnabled)                                  \
{                                                           \
    int xbyte = memoryBus->read(extendedPageAddress | ZPA); \
    if (xbyte & 0x80)                                       \
    {                                                       \
        xbyte &= 0x0f;                                      \
        systemControl->postMessage(this,                    \
            APPLEIII_SET_EXTENDEDRAMBANK, &xbyte);          \
        APPLEIIIRDMEM((EAH << 8) | ((EAL + Y) & 0xff));     \
        EAW += Y;                                           \
        tmp = APPLEIIIRDMEM_ID(EAA);                        \
    }                                                       \
    else                                                    \
    {                                                       \
        RDMEM((EAH << 8) | ((EAL + Y) & 0xff));             \
        EAW += Y;                                           \
        tmp = RDMEM_ID(EAA);                                \
    }                                                       \
}                                                           \
else                                                        \
{                                                           \
    RDMEM((EAH << 8) | ((EAL + Y) & 0xff));                 \
    EAW += Y;                                               \
    tmp = RDMEM_ID(EAA);                                    \
}

#define APPLEIIIWR_IDY_NP                                   \
ZPL = RDOPARG();											\
EAL = RDMEM(ZPA);											\
ZPL++;														\
EAH = RDMEM(ZPA);											\
if (extendedMemoryEnabled)                                  \
{                                                           \
    int xbyte = memoryBus->read(extendedPageAddress | ZPA); \
    if (xbyte & 0x80)                                       \
    {                                                       \
        xbyte &= 0x0f;                                      \
        systemControl->postMessage(this,                    \
            APPLEIII_SET_EXTENDEDRAMBANK, &xbyte);          \
        APPLEIIIRDMEM((EAH << 8) | ((EAL + Y) & 0xff));     \
        EAW += Y;                                           \
        APPLEIIIWRMEM_ID(EAA, tmp);                         \
    }                                                       \
    else                                                    \
    {                                                       \
        RDMEM((EAH << 8) | ((EAL + Y) & 0xff));             \
        EAW += Y;                                           \
        WRMEM_ID(EAA, tmp);                                 \
    }                                                       \
}                                                           \
else                                                        \
{                                                           \
    RDMEM((EAH << 8) | ((EAL + Y) & 0xff));                 \
    EAW += Y;                                               \
    WRMEM_ID(EAA, tmp);                                     \
}
