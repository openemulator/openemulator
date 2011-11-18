
/**
 * libemulator
 * Apple II Video Sync
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Synchronizes video with memory
 */

#include "OEComponent.h"

class AppleIIVideoSync : public OEComponent
{
public:
    AppleIIVideoSync();
    
	bool setRef(string name, OEComponent *ref);
    bool init();
    
	void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *video;
    OEComponent *ram;
};
