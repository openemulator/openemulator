
/**
 * libemulation
 * AudioPlayer Interface
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the audio player interface
 */

#ifndef _AUDIOPLAYERINTERFACE_H
#define _AUDIOPLAYERINTERFACE_H

#include "AudioPlayerInterface.h"

typedef enum
{
    AUDIOPLAYER_PLAY,
    AUDIOPLAYER_PAUSE,
    AUDIOPLAYER_SET_SOUND,
    AUDIOPLAYER_SET_VOLUME,
    AUDIOPLAYER_SET_LOOP,
} AudioPlayerMessage;

#endif
