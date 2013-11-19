
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
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    OEShort read16(OEAddress address);
    void write16(OEAddress address, OEShort value);
    
private:
    float timeAccuracy;
    float lowFrequency;
    float highFrequency;
    OEInt filterSize;
    
    OEComponent *audio;
    OEComponent *controlBus;
    
    AudioBuffer *audioBuffer;
    
    float sampleRate;
    OEInt channelNum;
    OEInt frameNum;
    OEInt sampleNum;
    
    OEInt impulseFilterSize;
    OEInt impulseFilterHalfSize;
    OEInt impulseTableEntryNum;
    OEInt impulseTableEntrySize;
    vector<float> impulseTable;
    
    vector<float> lastInput;
    vector<float> buffer;
    float integrationAlpha;
    vector<float> lastOutput;
    
    void updateSynth();
    void setSynth(float index, OEInt channel, float level);
    void synthBuffer();
};
