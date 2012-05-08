
/**
 * libemulation
 * Apple Language Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Language Card
 */

#include "AppleLanguageCard.h"

#include "ControlBusInterface.h"
#include "MemoryInterface.h"
#include "AppleIIInterface.h"

AppleLanguageCard::AppleLanguageCard()
{
    controlBus = NULL;
    memoryBus = NULL;
    ram = NULL;
    romF8 = NULL;
    
    bank1 = false;
    ramRead = false;
    preWrite = false;
    ramWrite = false;
}

bool AppleLanguageCard::setValue(string name, string value)
{
    if (name == "bank1")
        bank1 = getOEInt(value);
    else if (name == "ramRead")
        ramRead = getOEInt(value);
    else if (name == "preWrite")
        preWrite = getOEInt(value);
    else if (name == "ramWrite")
        ramWrite = getOEInt(value);
    else
        return false;
    
    return true;
}

bool AppleLanguageCard::getValue(string name, string& value)
{
    if (name == "bank1")
        value = getString(bank1);
    else if (name == "ramRead")
        value = getString(ramRead);
    else if (name == "preWrite")
        value = getString(preWrite);
    else if (name == "ramWrite")
        value = getString(ramWrite);
    else
        return false;
    
    return true;
}

bool AppleLanguageCard::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
    }
    else if (name == "memoryBus")
        memoryBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "ram")
        ram = ref;
    else if (name == "romF8")
        romF8 = ref;
    else
        return false;
    
    return true;
}

bool AppleLanguageCard::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!ram)
    {
        logMessage("ram not connected");
        
        return false;
    }
    
    setROMF8(true);
    updateBank1();
    updateRAMRead();
    updateRAMWrite();
    
    return true;
}

void AppleLanguageCard::dispose()
{
    setROMF8(false);
    setRAMRead(false);
    setRAMWrite(false);
}

void AppleLanguageCard::notify(OEComponent *sender, int notification, void *data)
{
    ControlBusPowerState powerState = *((ControlBusPowerState *)data);
    
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
    {
        setBank1(false);
        setRAMRead(false);
        preWrite = false;
        setRAMWrite(false);
    }
}

OEChar AppleLanguageCard::read(OEAddress address)
{
    setBank1(address & 0x8);
	
	if (address & 0x1)
    {
		if (preWrite)
            setRAMWrite(true);
	}
    else
        setRAMWrite(false);
	preWrite = address & 0x1;
	
    setRAMRead(!(((address & 2) >> 1) ^ (address & 1)));
    
    return floatingBus->read(address);
}

void AppleLanguageCard::write(OEAddress address, OEChar value)
{
    setBank1(address & 0x8);
	
	if (!(address & 0x1))
        setRAMWrite(false);
	preWrite = false;
	
    ramRead = !(((address & 2) >> 1) ^ (address & 1));
}

void AppleLanguageCard::setROMF8(bool value)
{
    MemoryMaps memoryMaps;
    MemoryMap memoryMap;
    
    memoryMap.component = romF8;
    memoryMap.startAddress = 0xf800;
    memoryMap.endAddress = 0xffff;
    memoryMap.read = true;
    memoryMap.write = false;
    memoryMaps.push_back(memoryMap);
    
    memoryBus->postMessage(this, (value ?
                                  APPLEII_MAP_MEMORYMAPS :
                                  APPLEII_UNMAP_MEMORYMAPS), &memoryMaps);
}

void AppleLanguageCard::setBank1(bool value)
{
    if (bank1 == value)
        return;
    
    bank1 = value;
    
    updateBank1();
}

void AppleLanguageCard::updateBank1()
{
    BankSwitchedRAMMap ramMap;
    
    ramMap.startAddress = 0x1000;
    ramMap.endAddress = 0x1fff;
    ramMap.offset = bank1 ? -0x1000 : 0x0000;
    
    ram->postMessage(this, BANKSWITCHEDRAM_MAP, &ramMap);
}

void AppleLanguageCard::setRAMRead(bool value)
{
    if (ramRead == value)
        return;
    
    ramRead = value;
    
    updateRAMRead();
}

void AppleLanguageCard::updateRAMRead()
{
    MemoryMaps memoryMaps;
    MemoryMap memoryMap;
    
    memoryMap.component = ram;
    memoryMap.startAddress = 0xd000;
    memoryMap.endAddress = 0xffff;
    memoryMap.read = true;
    memoryMap.write = false;
    memoryMaps.push_back(memoryMap);
    
    memoryBus->postMessage(this, (ramRead ?
                                  APPLEII_MAP_MEMORYMAPS :
                                  APPLEII_UNMAP_MEMORYMAPS), &memoryMaps);
}

void AppleLanguageCard::setRAMWrite(bool value)
{
    if (ramWrite == value)
        return;
    
    ramWrite = value;
    
    updateRAMWrite();
}

void AppleLanguageCard::updateRAMWrite()
{
    MemoryMaps memoryMaps;
    MemoryMap memoryMap;
    
    memoryMap.component = ram;
    memoryMap.startAddress = 0xd000;
    memoryMap.endAddress = 0xffff;
    memoryMap.read = false;
    memoryMap.write = true;
    memoryMaps.push_back(memoryMap);
    
    memoryBus->postMessage(this, (ramWrite ?
                                  APPLEII_MAP_MEMORYMAPS :
                                  APPLEII_UNMAP_MEMORYMAPS), &memoryMaps);
}
