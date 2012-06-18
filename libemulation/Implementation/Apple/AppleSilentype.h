
/**
 * libemulation
 * Apple Silentype
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype printer
 */

#include "OEComponent.h"
#include "OEImage.h"

class AppleSilentype : public OEComponent
{
public:
	AppleSilentype();
	
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
	OEComponent *device;
	OEComponent *canvas;
	
    bool data;
    bool shiftClock;
    bool storeClock;
    
    OEShort shiftRegister;
    OEShort parallelRegister;
    
    OEInt headDrivePhaseControl;
    OEInt paperDrivePhaseControl;
    OEInt headDotControl;
    
    OEPoint headPosition;
    
	string imagePath;
    
    void updateData();
    void updateMechanics();

    void updateStepper(float& position, OEInt phaseControl);
};
