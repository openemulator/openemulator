
/**
 * libemulation
 * Apple-1 ACI
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include "OEComponent.h"

class Apple1ACI : public OEComponent
{
public:
    Apple1ACI();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    void dispose();
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    
private:
    float volume;
    float noiseRejection;
    
    OEComponent *rom;
    OEComponent *mmu;
    OEComponent *audioCodec;
    
    OEUInt8 inputTriggerThreshold;
    OEUInt8 inputCurrentThreshold;
    
    bool outputState;
	OEInt16 outputHighLevel;
    
    void mapMMU(int message);
    void toggleSpeaker();
};
