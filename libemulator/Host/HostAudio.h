
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include "OEComponent.h"

#define HOSTSYSTEM_POWER		0x00
#define HOSTSYSTEM_PAUSE		0x01
#define HOSTSYSTEM_RESET		0x02
#define HOSTSYSTEM_INTERRUPT	0x03

// Messages
enum
{
	HOSTAUDIO_RENDERBUFFER = OEIOCTL_USER,
	HOSTAUDIO_GETBUFFER,
	HOSTAUDIO_ADD_RUNTIME,
	HOSTAUDIO_GET_RUNTIME,
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
	int ioctl(int message, void *data);
	
private:
	double runTime;
	bool isPaused;
};
