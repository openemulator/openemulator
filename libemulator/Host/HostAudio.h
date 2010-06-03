
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events and system state
 */

#include "OEComponent.h"

// Messages
enum
{
	HOSTAUDIO_RENDER_BUFFER,
	HOSTAUDIO_ADD_RUNTIME,
};

// Notifications
enum
{
	HOSTAUDIO_RENDER_WILL_START,
	HOSTAUDIO_RENDER_DID_START,
	HOSTAUDIO_RENDER_WILL_END,
	HOSTAUDIO_RENDER_DID_END,
	HOSTAUDIO_SYSTEM_DID_POWER_ON,
	HOSTAUDIO_SYSTEM_DID_PAUSE,
	HOSTAUDIO_SYSTEM_DID_POWER_OFF,
};

// States
enum
{
	HOSTAUDIO_SYSTEM_POWERED_ON = 0,
	HOSTAUDIO_SYSTEM_PAUSED = 1,
	HOSTAUDIO_SYSTEM_POWERED_OFF = 2,
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
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	int state;
	double runTime;
	string notes;
};
