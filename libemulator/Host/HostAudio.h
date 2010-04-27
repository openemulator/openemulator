
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include "OEComponent.h"

// Messages
enum
{
	HOSTAUDIO_RENDERBUFFER = OEIOCTL_USER,
	HOSTAUDIO_GETBUFFER,
};

// Notifications
enum
{
	HOSTAUDIO_RENDER_WILL_START,
	HOSTAUDIO_RENDER_DID_START,
	HOSTAUDIO_RENDER_WILL_END,
	HOSTAUDIO_RENDER_DID_END,
};

// Types
typedef struct
{
	float sampleRate;
	int channelNum;
	int frameNum;
	float *input;
	float *output;
} HostAudioBuffer;

class HostAudio : public OEComponent
{
public:
	HostAudio();
	
	int ioctl(int message, void *data);
	
private:
	float f;
	float phase;
};
