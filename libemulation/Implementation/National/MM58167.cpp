
/**
 * libemulation
 * MM58167
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MM58167 real time clock
 */

#include <time.h>
#include <sys/time.h>

#include "MM58167.h"

#include "AudioInterface.h"

MM58167::MM58167()
{
    audio = NULL;
    
    ram.resize(8);
    
    deltaSec = 0;
    deltaUSec = 0;
    
    interruptFlags = 0;
    interruptControl = 0;
}

bool MM58167::setRef(string name, OEComponent *ref)
{
    if (name == "audio")
    {
        if (audio)
            audio->removeObserver(this, AUDIO_BUFFER_WILL_RENDER);
        audio = ref;
        if (audio)
            audio->addObserver(this, AUDIO_BUFFER_WILL_RENDER);
    }
    else
        return false;
    
    return true;
}

void MM58167::notify(OEComponent *sender, int notification, void *data)
{
    // Verify interrupt
}

bool MM58167::init()
{
    ramp = &ram.front();
    
    return true;
}

OEChar MM58167::read(OEAddress address)
{
    logMessage("MM58167 R " + getHexString(address & 0x1f));
    
    timeval unixTime;
    tm *localTime;
    
    if (address < 8)
    {
        gettimeofday(&unixTime, NULL);
        
        unixTime.tv_sec += deltaSec;
        unixTime.tv_usec += deltaUSec;
        
        localTime = localtime(&unixTime.tv_sec);
    }
    
    switch (address & 0x1f)
    {
        case 0x00:
            return getBCD(10 * ((unixTime.tv_usec / 1000) % 10));
            
        case 0x01:
            return getBCD(unixTime.tv_usec / 10000);
            
        case 0x02:
            return getBCD(localTime->tm_sec);
            
        case 0x03:
            return getBCD(localTime->tm_min);
            
        case 0x04:
            return getBCD(localTime->tm_hour);
            
        case 0x05:
            return getBCD(localTime->tm_wday);
            
        case 0x06:
            return getBCD(localTime->tm_mday);
            
        case 0x07:
            return getBCD(localTime->tm_mon);
            
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            return ram[address & 0x7];
            
        case 0x10:
        {
            OEChar value = interruptFlags;
            
            interruptFlags = 0;
            
            return value;
        }
        case 0x11:
            return interruptControl;
            
        case 0x14:
            // Status bit
            return 0;
    }
    
    return 0;
}

void MM58167::write(OEAddress address, OEChar value)
{
    logMessage("MM58167 W " + getHexString(address & 0x1f) + ": " + getHexString(value));
    
    timeval unixTime;
    tm *localTime;
    
    if (address < 8)
    {
        gettimeofday(&unixTime, NULL);
        
        unixTime.tv_sec += deltaSec;
        unixTime.tv_usec += deltaUSec;
        
        localTime = localtime(&unixTime.tv_sec);
    }
    
    switch (address & 0x1f)
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            // Write
            break;
            
        case 0x08:
            ram[address & 0x7] = value & 0xf0;
            
            break;
            
        case 0x09:
        case 0x0a:
        case 0x0b:
            ram[address & 0x7] = value;
            
            break;
            
        case 0x0c:
            ram[address & 0x7] = value & 0x0f;
            
            break;
            
        case 0x0d:
        case 0x0e:
        case 0x0f:
            ram[address & 0x7] = value;
            
            break;
            
        case 0x11:
            interruptControl = value;
            
            break;
            
        case 0x12:
            // Counter reset
            
            break;
            
        case 0x13:
            // RAM reset
            for (OEInt i = 0; i < 8; i++)
                ram[i] = 0;
            
            break;
            
        case 0x15:
            // Go command
            
            break;
    }
}

OEChar MM58167::getBCD(OEChar value)
{
    return ((value / 10) << 4) | (value % 10);
}
