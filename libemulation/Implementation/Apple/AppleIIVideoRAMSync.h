
/**
 * libemulator
 * Apple II Video RAM Sync
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Synchronizes video with video RAM
 */

#include "OEComponent.h"

class AppleIIVideoRAMSync : public OEComponent
{
public:
    AppleIIVideoRAMSync();
    
	bool setRef(string name, OEComponent *ref);
    bool init();
    
	void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *video;
    OEComponent *ram1;
    OEComponent *ram2;
};
