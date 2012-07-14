
/**
 * libemulation
 * ATA device
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an ATA device
 */

#include "DeviceInterface.h"
#include "StorageInterface.h"

#include "ATADevice.h"

ATADevice::ATADevice()
{
    device = NULL;
}

ATADevice::~ATADevice()
{
    closeDiskImage();
}

bool ATADevice::setValue(string name, string value)
{
    if (name == "diskImage")
        openDiskImage(value);
    else if (name == "forceWriteProtected")
        blockStorage.setForceWriteProtected(getOEInt(value));
    else if (name == "maxSize")
        blockStorage.setMaxSize(getOEInt(value));
    else
        return false;
    
    return true;
}

bool ATADevice::getValue(string name, string& value)
{
    if (name == "diskImage")
        value = blockStorage.getPath();
    else if (name == "forceWriteProtected")
        value = getString(blockStorage.getForceWriteProtected());
    else
        return false;
    
    return true;
}

bool ATADevice::setRef(string name, OEComponent *ref)
{
    if (name == "device")
    {
        if (device)
            device->postMessage(this, DEVICE_REMOVE_STORAGE, this);
        device = ref;
        if (device)
            device->postMessage(this, DEVICE_ADD_STORAGE, this);
    }
    else
        return false;
    
    return true;
}

bool ATADevice::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    return true;
}

bool ATADevice::postMessage(OEComponent *sender, int message, void *data)
{
    switch(message)
    {
        case STORAGE_IS_AVAILABLE:
            return true;
            
        case STORAGE_CAN_MOUNT:
        {
            DIATABlockStorage blockStorage;
            
            return blockStorage.open(*((string *)data));
        }
            
        case STORAGE_MOUNT:
            if (openDiskImage(*((string *)data)))
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return true;
            }
            
            return false;
            
        case STORAGE_UNMOUNT:
            closeDiskImage();
            
            device->postMessage(this, DEVICE_UPDATE, NULL);
            
            return true;
            
        case STORAGE_GET_MOUNTPATH:
            *(string *)data = blockStorage.getPath();
            
            return true;
            
        case STORAGE_IS_LOCKED:
            return false;
            
        case STORAGE_GET_FORMATLABEL:
            *((string *)data) = blockStorage.getFormatLabel();
            
            return true;
            
        case STORAGE_GET_OBJECT:
            *((DIATABlockStorage **)data) = &blockStorage;
            
            return true;
    }
    
    return false;
}

bool ATADevice::openDiskImage(string path)
{
    closeDiskImage();
    
    if (!blockStorage.open(path))
        return false;
    
    return true;
}

void ATADevice::closeDiskImage()
{
    blockStorage.close();
}
