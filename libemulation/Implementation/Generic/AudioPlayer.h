
/**
 * libemulation
 * Audio player
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio player
 */

#include "OEComponent.h"

#include "OESound.h"

#include "samplerate.h"

class AudioPlayer : public OEComponent
{
public:
    AudioPlayer();
    
    bool setValue(string name, string value);
    bool getValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *audio;
    
    bool playing;
    bool loop;
    float volume;
    OEUInt64 frameIndex;
    
    OESound *sound;
    OESound loadedSound;
    
    int srcChannelNum;
    SRC_STATE *srcState;
    bool srcEndOfInput;
};
