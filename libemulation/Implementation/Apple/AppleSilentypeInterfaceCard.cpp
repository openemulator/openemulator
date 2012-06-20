
/**
 * libemulation
 * Apple Silentype Interface Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype Interface Card
 */

#include "AppleSilentypeInterfaceCard.h"

#include "MemoryInterface.h"
#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

/*
 * Internal printer protocol uses:
 *
 * (Write)
 *  Bit 0: Data
 *  Bit 1: Shift clock
 *  Bit 2: Store clock
 *
 * (Read)
 *  Bit 0: Data
 *  Bit 1: Shift clock
 *  Bit 2: Machine Status
 */

#define SILENTYPE_WRITE                 (1 << 0)
#define SILENTYPE_READ_DATA             (1 << 1)
#define SILENTYPE_READ_MACHINESTATUS    (1 << 2)

#define SILENTYPE_DATA                  (1 << 0)
#define SILENTYPE_SHIFTCLOCKB           (1 << 1)
#define SILENTYPE_STORECLOCK            (1 << 2)
#define SILENTYPE_DATAWRITEENABLED      (1 << 3)
#define SILENTYPE_SHIFTCLOCKA           (1 << 4)
#define SILENTYPE_ROMENABLED            (1 << 5)
#define SILENTYPE_SHIFTCLOCKDISABLED    (1 << 6)

#define SILENTYPE_SHIFTCLOCKREAD        (1 << 6)
#define SILENTYPE_STATUS                (1 << 7)

#define SERIAL_DATA                     (1 << 0)
#define SERIAL_SHIFTCLOCK               (1 << 1)
#define SERIAL_STORECLOCK               (1 << 2)
#define SERIAL_MACHINESTATUS            (1 << 2)

AppleSilentypeInterfaceCard::AppleSilentypeInterfaceCard()
{
    controlBus = NULL;
    floatingBus = NULL;
    slotController = NULL;
    memoryCF00 = NULL;
    memoryMapper = NULL;
    printer = NULL;
}

bool AppleSilentypeInterfaceCard::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
    }
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "slotController")
    {
        if (slotController)
            slotController->removeObserver(this, APPLEII_C800_DID_CHANGE);
        slotController = ref;
        if (slotController)
            slotController->addObserver(this, APPLEII_C800_DID_CHANGE);
    }
    else if (name == "memoryCF00")
        memoryCF00 = ref;
    else if (name == "memoryMapper")
        memoryMapper = ref;
    else if (name == "printer")
        printer = ref;
    else
        return false;
    
    return true;
}

bool AppleSilentypeInterfaceCard::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!memoryCF00)
    {
        logMessage("memoryCF00 not connected");
        
        return false;
    }
    
    return true;
}

bool AppleSilentypeInterfaceCard::postMessage(OEComponent *sender, int message, void *data)
{
    return false;
}

void AppleSilentypeInterfaceCard::notify(OEComponent *sender, int notification, void *data)
{
    if (printer)
        printer->write(0, 0);
    
    setROMEnabled(false);
}

OEChar AppleSilentypeInterfaceCard::read(OEAddress address)
{
    OEChar state = 0;
    
    if (printer)
        state = printer->read(0);
    
    if (OEGetBit(address, SILENTYPE_READ_DATA))
    {
        OEChar value = floatingBus->read(address) & 0x3f;
        OESetBit(value, SILENTYPE_SHIFTCLOCKREAD, OEGetBit(state, SERIAL_SHIFTCLOCK));
        OESetBit(value, SILENTYPE_STATUS, OEGetBit(state, SERIAL_DATA));
        
        return value;
    }
    
    if (OEGetBit(address, SILENTYPE_READ_MACHINESTATUS))
    {
        OEChar value = floatingBus->read(address) & 0x3f;
        OESetBit(value, SILENTYPE_SHIFTCLOCKREAD, OEGetBit(state, SERIAL_SHIFTCLOCK));
        OESetBit(value, SILENTYPE_STATUS, OEGetBit(state, SERIAL_MACHINESTATUS));
        
        return value;
    }
    
    return floatingBus->read(address);
}

void AppleSilentypeInterfaceCard::write(OEAddress address, OEChar value)
{
//    logMessage("w " + getHexString(value));
    
    if (OEGetBit(address, SILENTYPE_WRITE))
    {
        setROMEnabled(OEGetBit(value, SILENTYPE_ROMENABLED));
        
        bool data = (OEGetBit(value, SILENTYPE_DATA) &&
                     OEGetBit(value, SILENTYPE_DATAWRITEENABLED));
        bool storeClock = !OEGetBit(value, SILENTYPE_STORECLOCK);
        
        bool shiftClock = (!OEGetBit(value, SILENTYPE_SHIFTCLOCKA) &&
                           !OEGetBit(value, SILENTYPE_SHIFTCLOCKDISABLED));
        
        OEChar state = 0;
        OESetBit(state, SERIAL_DATA, data);
        OESetBit(state, SERIAL_SHIFTCLOCK, shiftClock);
        OESetBit(state, SERIAL_STORECLOCK, storeClock);
        if (printer)
            printer->write(0, state);
        
        shiftClock = (!(OEGetBit(value, SILENTYPE_SHIFTCLOCKA) ||
                        OEGetBit(value, SILENTYPE_SHIFTCLOCKB)) &&
                      !OEGetBit(value, SILENTYPE_SHIFTCLOCKDISABLED));
        
        OESetBit(state, SERIAL_SHIFTCLOCK, shiftClock);
        if (printer)
            printer->write(0, state);
        
        shiftClock = false;
        OESetBit(state, SERIAL_SHIFTCLOCK, shiftClock);
        if (printer)
            printer->write(0, state);
    }
}

void AppleSilentypeInterfaceCard::setROMEnabled(bool value)
{
    string m = value ? "ROM" : "RAM";
    
    memoryMapper->postMessage(this, ADDRESSMAPPER_MAP, &m);
}
