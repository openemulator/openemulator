
/**
 * libemulation
 * Apple Silentype
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype printer
 */

#include "AppleSilentype.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"

#define PARALLEL_READ   (1 << 8)
#define PARALLEL_Q15    (1 << 15)

#define PAPER_WIDTH         800
#define DOT_NUM             7

AppleSilentype::AppleSilentype()
{
	device = NULL;
	canvas = NULL;
    
    data = false;
    shiftClock = false;
    storeClock = false;
    
    shiftRegister = 0;
    parallelRegister = 0;
    
    headDrivePhaseControl = 0;
    paperDrivePhaseControl = 0;
    headDotControl = 0;
    
    printPosition = OEMakePoint(0, 0);
}

bool AppleSilentype::setValue(string name, string value)
{
    if (name == "shiftRegister")
        shiftRegister = getOEInt(value);
    else if (name == "parallelRegister")
        parallelRegister = getOEInt(value);
    else
        return false;
    
    return true;
}

bool AppleSilentype::getValue(string name, string &value)
{
    if (name == "shiftRegister")
        value = getHexString(shiftRegister);
    else if (name == "parallelRegister")
        value = getHexString(parallelRegister);
    else
        return false;
    
    return true;
}

bool AppleSilentype::setRef(string name, OEComponent *ref)
{
	if (name == "device")
	{
		if (device)
        {
            if (canvas)
                canvas->removeObserver(this, CANVAS_DID_DELETE);
			device->postMessage(this,
                                DEVICE_DESTROY_CANVAS,
                                &canvas);
        }
		device = ref;
		if (device)
        {
			device->postMessage(this,
                                DEVICE_CONSTRUCT_PAPERCANVAS,
                                &canvas);
            if (canvas)
                canvas->addObserver(this, CANVAS_DID_DELETE);
        }
	}
	else
		return false;
	
	return true;
}

bool AppleSilentype::init()
{
	if (!device)
	{
		logMessage("property 'device' undefined");
		return false;
	}
	
	if (!canvas)
	{
		logMessage("canvas could not be created");
		return false;
	}
	
	CanvasPaperConfiguration configuration;
	configuration.pageResolution = OEMakeSize(612 * 2, 792);
	configuration.pixelDensity = OEMakeSize(144, 72);
	canvas->postMessage(this, CANVAS_CONFIGURE_PAPER, &configuration);
	
    updateData();
    updateParallel();
    
	return true;
}

void AppleSilentype::notify(OEComponent *sender, int notification, void *data)
{
    canvas->postMessage(this, CANVAS_CLEAR, NULL);
    
    printPosition.y = 0;
}

OEChar AppleSilentype::read(OEAddress address)
{
//    logMessage("r: " + getHexString(address));
    
    bool machineStatus = (printPosition.x < 0);
    
    OEChar value = 0;
    
    OESetBit(value, (1 << 0), data);
    OESetBit(value, (1 << 1), shiftClock);
    OESetBit(value, (1 << 2), machineStatus);
    
    return value;
}

void AppleSilentype::write(OEAddress address, OEChar value)
{
//    logMessage("w: " + getHexString(value));
    
    data = OEGetBit(value, (1 << 0));
    bool newShiftClock = OEGetBit(value, (1 << 1));
    bool newStoreClock = OEGetBit(value, (1 << 2));
    
    if (shiftClock != newShiftClock)
    {
        shiftClock = newShiftClock;
        
        // Positive-edge?
        if (shiftClock)
        {
            // Shift
            shiftRegister <<= 1;
            
            updateData();
            
            if (!OEGetBit(parallelRegister, PARALLEL_READ))
                shiftRegister |= data;
        }
    }
    
    if (storeClock != newStoreClock)
    {
        storeClock = newStoreClock;
        
        // Positive-edge?
        if (storeClock && !OEGetBit(parallelRegister, PARALLEL_READ))
        {
            // Parallel load
            parallelRegister = shiftRegister;
            
            updateData();
            updateParallel();
        }
    }
    
    // Shift load
    if (storeClock && OEGetBit(parallelRegister, PARALLEL_READ))
    {
        shiftRegister = parallelRegister;
        
        updateData();
    }
    
    // Store clear
    if (shiftClock && storeClock)
    {
        parallelRegister = 0;
        
        updateData();
        updateParallel();
    }
}

void AppleSilentype::updateData()
{
    if (OEGetBit(parallelRegister, PARALLEL_READ))
        data = OEGetBit(shiftRegister, PARALLEL_Q15);
}

void AppleSilentype::updateParallel()
{
    headDrivePhaseControl = (parallelRegister >> 0) & 0xf;
    paperDrivePhaseControl = (parallelRegister >> 4) & 0xf;
    headDotControl = (parallelRegister >> 9) & 0x7f;
    
    updateStepper(printPosition.x, headDrivePhaseControl);
    updateStepper(printPosition.y, paperDrivePhaseControl);
    
    if (printPosition.y < 0)
        return;
    
    if ((printPosition.x < 0) || (printPosition.x >= PAPER_WIDTH))
        return;
    
    OEImage image;
    image.setSize(OEMakeSize(1, DOT_NUM));
    
    for (OEInt y = 0; y < DOT_NUM; y++)
    {
        bool value = OEGetBit(headDotControl, (0x40 >> y));
        
        image.setPixel(0, y, OEColor(value ? 0x80 : 0xff));
    }
    
    for (OEInt y = 0; y < 16; y++)
    {
        bool value = OEGetBit(parallelRegister, (1 << y));
        
        printf(value ? "." : " ");
    }
    
    printf("\n");
    
    canvas->postMessage(this, CANVAS_SET_PRINTPOSITION, &printPosition);
    canvas->postMessage(this, CANVAS_POST_IMAGE, &image);
    
/*    logMessage("headPosition: (" +
               getString(printPosition.x) + ", " +
               getString(printPosition.y) + ")");*/
}

void AppleSilentype::updateStepper(float& position, OEInt phaseControl)
{
    float invPosition = -position;
    
	OESInt currentPhase = ((OESInt) invPosition) & 0x7;
	OESInt nextPhase;
	
	switch (phaseControl)
    {
		case 0x1: case 0xb:
			nextPhase = 0;
            
			break;
            
		case 0x3:
			nextPhase = 1;
            
			break;
            
		case 0x2: case 0x7:
			nextPhase = 2;
            
			break;
            
		case 0x6:
			nextPhase = 3;
            
			break;
            
		case 0x4: case 0xe:
			nextPhase = 4;
            
			break;
            
		case 0xc:
			nextPhase = 5;
            
			break;
            
		case 0x8: case 0xd:
			nextPhase = 6;
            
			break;
            
		case 0x9:
			nextPhase = 7;
            
			break;
            
		default:
			nextPhase = currentPhase;
            
			break;
	}
    
    invPosition += ((nextPhase - currentPhase + 4) & 0x7) - 4;
    
    position = -invPosition;
}
