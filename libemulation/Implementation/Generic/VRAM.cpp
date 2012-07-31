
/**
 * libemulation
 * Video RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls video random access memory
 */

#include "VRAM.h"

#include "MemoryInterface.h"

VRAM::VRAM() : RAM()
{
    videoObserver = NULL;
}

bool VRAM::setValue(string name, string value)
{
    if (name == "videoBlockSize")
		videoBlockSize = getOEInt(value);
    else if (name == "videoMap")
        videoMap = value;
    else
        return RAM::setValue(name, value);
    
    return true;
}

bool VRAM::setRef(string name, OEComponent *ref)
{
    if (name == "videoObserver")
        videoObserver = ref;
    else
        return RAM::setRef(name, ref);
    
    return true;
}

bool VRAM::init()
{
    if (!RAM::init())
        return false;
    
    if (!videoObserver)
    {
		logMessage("videoObserver not connected");
        
		return false;
    }
    
    if ((videoBlockSize != getNextPowerOf2(videoBlockSize)) ||
        (size < videoBlockSize) ||
        (!videoBlockSize) ||
        (!size))
    {
		logMessage("invalid value for size/videoBlockSize");
        
		return false;
    }
    
    videoBlockBits = getBitNum(videoBlockSize);
	
	size_t videoBlockNum = (size_t) (size / videoBlockSize);
	notifyMap.resize(videoBlockNum);
    
    notifyMapp = &notifyMap.front();
    
    MemoryMaps theMaps;
    
    appendMemoryMaps(theMaps, NULL, videoMap);
    
    for (MemoryMaps::iterator i = theMaps.begin();
         i != theMaps.end();
         i++)
    {
        size_t startBlock = (size_t) (i->startAddress >> videoBlockBits);
        size_t endBlock = (size_t) (i->endAddress >> videoBlockBits);
        
        for (size_t i = startBlock; i <= endBlock; i++)
            notifyMap[i] = true;
    }
    
    return true;
}

void VRAM::write(OEAddress address, OEChar value)
{
    address &= mask;
    
    if (notifyMapp[address >> videoBlockBits])
        videoObserver->notify(this, VRAM_WILL_CHANGE, NULL);
    
    datap[address] = value;
}
