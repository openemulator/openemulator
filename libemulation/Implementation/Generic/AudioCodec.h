
/**
 * libemulation
 * Audio codec
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an 8/16 bit audio codec (ADC/DAC)
 */

#include "OEComponent.h"
#include "AudioInterface.h"

class AudioCodec : public OEComponent
{
public:
    AudioCodec();
    
    bool setRef(string name, OEComponent *ref);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    OEUInt16 read16(OEAddress address);
    void write16(OEAddress address, OEUInt16 value);
    
private:
    OEComponent *audio;
    OEComponent *controlBus;
    
    AudioBuffer *audioBuffer;
};
