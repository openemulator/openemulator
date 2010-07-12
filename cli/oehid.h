
/**
 * OpenEmulator
 * OpenEmulator/HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/HID interface.
 */

#define OEHID_KEYDOWN_SIZE		512
#define OEHID_MOUSEDOWN_SIZE	8
#define OEHID_JOYSTICKDOWN_SIZE	64

typedef struct
{
	bool keyDown[OEHID_KEYDOWN_SIZE];
	bool mouseDown[OEHID_MOUSEDOWN_SIZE];
	bool joystickDown;
	
	BOOL mouseButtonState[DOCUMENT_MOUSE_BUTTONNUM];
	BOOL mouseCaptured;
	BOOL mouseCaptureRelease;
} OEHIDContext

OEHIDContext *oehidOpen();
void oehidClose(OEHIDContext *context);

void oehidSendUnicode(OEHIDContext *context);
