
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
    floatingBus = NULL;
    bankSwitcher = NULL;
    
    bank1 = false;
    ramRead = false;
    preWrite = false;
    ramWrite = false;
    
    titanIII = false;
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
    else if (name == "titanIII")
        titanIII = getOEInt(value);
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
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "io")
        io = ref;
    else
        return false;
    
    return true;
}

bool AppleLanguageCard::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(memoryBus);
    OECheckComponent(floatingBus);
    OECheckComponent(bankSwitcher);
    
    ramMap.component = bankSwitcher;
    ramMap.startAddress = 0xd000;
    ramMap.endAddress = 0xffff;
    ramMap.read = false;
    ramMap.write = false;
    
    updateBankSwitcher();
    updateBankOffset();
    
    if (titanIII && io)
    {
        slot0Map.component = this;
        slot0Map.startAddress = 0x80;
        slot0Map.endAddress = 0x8f;
        slot0Map.read = true;
        slot0Map.write = true;
        
        io->postMessage(this, ADDRESSDECODER_MAP, &slot0Map);
    }
    
    return true;
}

void AppleLanguageCard::dispose()
{
    if (titanIII && io)
        io->postMessage(this, ADDRESSDECODER_UNMAP, &slot0Map);
    
    ramRead = false;
    ramWrite = false;
    
    updateBankSwitcher();
}

void AppleLanguageCard::notify(OEComponent *sender, int notification, void *data)
{
    ControlBusPowerState powerState = *((ControlBusPowerState *)data);
    
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
    {
        bank1 = false;
        ramRead = false;
        preWrite = false;
        ramWrite = false;
        
        updateBankSwitcher();
        updateBankOffset();
    }
}

OEChar AppleLanguageCard::read(OEAddress address)
{
    if (titanIII && (address & 0x4))
    {
        logMessage("R " + getHexString(address));
        
        return floatingBus->read(address);
    }
    
	if (address & 0x1)
    {
		if (preWrite)
            ramWrite = true;
	}
    else
        ramWrite = false;
	preWrite = address & 0x1;
	
    ramRead = !(((address >> 1) ^ address) & 1);
    
    setBank1(address & 0x8);
    
    updateBankSwitcher();
    
    return floatingBus->read(address);
}

void AppleLanguageCard::write(OEAddress address, OEChar value)
{
    if (titanIII && (address & 0x4))
    {
        logMessage("W " + getHexString(address) + ": " + getHexString(value));
        
        return;
    }
    
	if (!(address & 0x1))
        ramWrite = false;
	preWrite = false;
	
    ramRead = !(((address >> 1) ^ address) & 1);
    
    setBank1(address & 0x8);
    
    updateBankSwitcher();
}

void AppleLanguageCard::setBank1(bool value)
{
    if (bank1 == value)
        return;
    
    bank1 = value;
    
    updateBankOffset();
}

void AppleLanguageCard::updateBankOffset()
{
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x1000;
    offsetMap.endAddress = 0x1fff;
    offsetMap.offset = bank1 ? -0x1000 : 0x0000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}

void AppleLanguageCard::updateBankSwitcher()
{
    if ((ramMap.read == ramRead) &&
        (ramMap.write == ramWrite))
        return;
    
    if (ramMap.read || ramMap.write)
        memoryBus->postMessage(this, ADDRESSDECODER_UNMAP, &ramMap);
    
    ramMap.read = ramRead;
    ramMap.write = ramWrite;
    
    if (ramMap.read || ramMap.write)
        memoryBus->postMessage(this, ADDRESSDECODER_MAP, &ramMap);
}
