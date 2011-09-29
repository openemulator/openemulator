
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
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    OEUInt16 read16(OEAddress address);
    void write16(OEAddress address, OEUInt16 value);
    
private:
    OEComponent *audio;
    OEComponent *controlBus;
    
    float timeAccuracy;
    float lowFrequency;
    float highFrequency;
    OEUInt32 filterSize;
    
    AudioBuffer *audioBuffer;
    
    float sampleRate;
    OEUInt32 channelNum;
    OEUInt32 frameNum;
    OEUInt32 sampleNum;
    
    OEUInt32 impulseFilterSize;
    OEUInt32 impulseFilterHalfSize;
    OEUInt32 impulseTableEntryNum;
    OEUInt32 impulseTableEntrySize;
    vector<float> impulseTable;
    
    vector<float> lastInput;
    vector<float> buffer;
    float integrationAlpha;
    vector<float> lastOutput;
    
    void updateSynth();
    void setSynth(float index, OEUInt32 channel, float level);
    void synthBuffer();
};
