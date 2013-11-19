
/**
 * libemulation
 * Audio interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the audio interface
 */

// Notes:
// * Buffer notifications send AudioBuffer

#ifndef _AUDIOINTERFACE_H
#define _AUDIOINTERFACE_H

#include "OECommon.h"

typedef enum
{
    AUDIO_BUFFER_WILL_RENDER,
    AUDIO_BUFFER_IS_RENDERING,
    AUDIO_BUFFER_DID_RENDER,
} AudioNotification;

typedef struct
{
    float sampleRate;
    OEInt channelNum;
    OEInt frameNum;
    const float *input;
    float *output;
} AudioBuffer;

float getLevelFromVolume(float value);

#endif
