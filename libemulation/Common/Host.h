
/**
 * libemulation
 * Host
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host events
 */

#include "OEComponent.h"

#include "HostHID.h"

// Notifications
enum
{
	HOST_POWERSTATE_DID_CHANGE,
	
	HOST_AUDIO_RENDER_WILL_BEGIN,
	HOST_AUDIO_RENDER_DID_BEGIN,
	HOST_AUDIO_RENDER_WILL_END,
	HOST_AUDIO_RENDER_DID_END,

	HOST_HID_SYSTEM_EVENT,
	HOST_HID_KEYBOARD_EVENT,
	HOST_HID_UNICODEKEYBOARD_EVENT,
	HOST_HID_POINTER_EVENT,
	HOST_HID_MOUSE_EVENT,
	HOST_HID_JOYSTICK1_EVENT,
	HOST_HID_JOYSTICK2_EVENT,
	HOST_HID_JOYSTICK3_EVENT,
	HOST_HID_JOYSTICK4_EVENT,
	HOST_HID_TABLET_EVENT,
	
	HOST_CLIPBOARD_COPY_EVENT,
	HOST_CLIPBOARD_PASTE_EVENT,
};

// Power States
enum
{
	HOST_POWERSTATE_ON = 0,
	HOST_POWERSTATE_PAUSE,
	HOST_POWERSTATE_STANDBY,
	HOST_POWERSTATE_SLEEP,
	HOST_POWERSTATE_HIBERNATE,
	HOST_POWERSTATE_OFF,
};

// Messages
enum
{
	HOST_ADD_SCREEN,
	HOST_REMOVE_SCREEN,
	HOST_UPDATE_SCREEN,
	HOST_GET_VIDEO_UPDATE,
	HOST_GET_SCREENS,
	
	HOST_IS_COPYABLE,
	HOST_IS_PASTEABLE,
};

// Devices - API to be determined
/*enum
{
	HOST_SERIALPORT1,
	HOST_SERIALPORT2,
	HOST_SERIALPORT3,
	HOST_SERIALPORT4,
	HOST_PARALLELPORT1,
	HOST_PARALLELPORT2,
	HOST_MIDI,
	HOST_ETHERNET1,
	HOST_ETHERNET2,
	HOST_USB,
	HOST_CAMERA,
};*/

// Data types
typedef struct
{
	float sampleRate;
	int channelNum;
	int frameNum;
	
	float *input;
	float *output;
} HostAudioBuffer;

typedef struct
{
	int *framebufferData;
	int framebufferWidth;
	int framebufferHeight;
	int contentWidth;
	int contentHeight;
	int screenWidth;
	int screenHeight;
	bool updated;
} HostVideoScreen;

typedef vector<HostVideoScreen *> HostVideoScreens;

class Host : public OEComponent
{
public:
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	string notes;
	
	int powerState;
	
	string videoWindow;
	bool videoUpdate;
	HostVideoScreens videoScreens;
	
	bool hidMouseCapture;
	int hidKeyboardLEDs;
	
	bool addScreen(HostVideoScreen *screen);
	bool removeScreen(HostVideoScreen *screen);
};
