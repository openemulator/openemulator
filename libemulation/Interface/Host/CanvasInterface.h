
/**
 * libemulation
 * Canvas interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the canvas interface.
 */

#ifndef _CANVASINTERFACE_H
#define _CANVASINTERFACE_H

#include "OEImage.h"

// Messages:
// * setMode sets the canvas mode (CanvasMode):
//   - display (fixed aspect ratio)
//   - paper (large scrolling areas)
//   - OpenGL (custom graphics)
// * setCaptureMode describes how the mouse and keyboard are captured (CanvasCaptureMode)
// * setKeyboardFlags sets the keyboard LEDs (CanvasKeyboardFlags)
// * setBezel sets the canvas' bezel (CanvasBezel)

// Configuration:
// * configureDisplay configures a display canvas (CanvasDisplayConfiguration)
// * configurePaper configures a display canvas (CanvasPaperConfiguration)
// * configureOpenGL configures a display canvas (CanvasOpenGLConfiguration)

// Drawing:
// * postImage post an image to the canvas (OEImage)
// * movePrintHead sets the print head position in paper mode (OEPoint)

// Notifications:
// * HID notifications are sents as CanvasHIDNotification
// * Pointer coordinates are in resolution coordinates
// * didCopy, didPaste is sent when copying/pasting (string)
// * didVSync is called from the video thread after vertical sync (CanvasVSync)
// * willDraw is called from the video thread when the screen will be drawn (OESize)
// * HID axes are in [-1:1] coordinates

// Multithreading, beware!
// * didVSync and didUpdate are sent from the drawing thread! Keep this in mind
//   for synchronizing correctly.

typedef enum
{
	CANVAS_SET_MODE,
	CANVAS_SET_CAPTUREMODE,
	CANVAS_SET_KEYBOARDFLAGS,
	CANVAS_SET_BEZEL,
	
	CANVAS_CONFIGURE_DISPLAY,
	CANVAS_CONFIGURE_PAPER,
	CANVAS_CONFIGURE_OPENGL,
	
	CANVAS_POST_IMAGE,
	CANVAS_MOVE_PRINTHEAD,
} CanvasMessage;

typedef enum
{
	CANVAS_KEYBOARD_DID_CHANGE,
	CANVAS_UNICODEKEYBOARD_DID_CHANGE,
	CANVAS_POINTER_DID_CHANGE,
	CANVAS_MOUSE_DID_CHANGE,
	CANVAS_JOYSTICK1_DID_CHANGE,
	CANVAS_JOYSTICK2_DID_CHANGE,
	CANVAS_JOYSTICK3_DID_CHANGE,
	CANVAS_JOYSTICK4_DID_CHANGE,
	
	CANVAS_DID_COPY,
	CANVAS_DID_PASTE,
	
	CANVAS_DID_VSYNC,
	CANVAS_WILL_DRAW,
} CanvasNotification;

// Definitions

typedef enum
{
	CANVAS_MODE_DISPLAY,
	CANVAS_MODE_PAPER,
	CANVAS_MODE_OPENGL,
} CanvasMode;

typedef enum
{
	CANVAS_CAPTUREMODE_NO_CAPTURE,
	CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK,
	CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER,
} CanvasCaptureMode;

// Canvas keyboard flags
typedef OEUInt32 CanvasKeyboardFlags;
#define CANVAS_L_NUMLOCK	(1 << 0)
#define CANVAS_L_CAPSLOCK	(1 << 1)
#define CANVAS_L_SCROLLLOCK	(1 << 2)
#define CANVAS_L_COMPOSE	(1 << 3)
#define CANVAS_L_KANA		(1 << 4)
#define CANVAS_L_POWER		(1 << 5)
#define CANVAS_L_SHIFT		(1 << 6)

// Canvas bezels
typedef enum
{
	CANVAS_BEZEL_NONE,
	CANVAS_BEZEL_POWER,
	CANVAS_BEZEL_PAUSE,
} CanvasBezel;

// Copy and paste use string

// Display canvas configuration:
// * displayResolution is the number of resolved pixels of the device (this
//   is not necessarily the video frame resolution)
// * displayPixelDensity is the number of resolved pixels of the device per inch
// * The video rectangle sets the bounds of the video frame within the canvas
//   (origin is upper left, coordinate system is [0..1, 0..1])
// * The shadow mask dot pitch measures the spacing of the pixels in mm
// * Video bandwidths are relative to the image sampling frequency,
//   not to the display resolution

typedef enum
{
	CANVAS_DECODER_RGB,
	CANVAS_DECODER_MONOCHROME,
	CANVAS_DECODER_NTSC_YIQ,
	CANVAS_DECODER_NTSC_CXA2025AS,
	CANVAS_DECODER_NTSC_YUV,
	CANVAS_DECODER_PAL,
} CanvasDecoder;

typedef enum
{
	CANVAS_SHADOWMASK_TRIAD,
	CANVAS_SHADOWMASK_INLINE,
	CANVAS_SHADOWMASK_APERTURE,
	CANVAS_SHADOWMASK_LCD,
	CANVAS_SHADOWMASK_BAYER,
} CanvasShadowMask;

class CanvasDisplayConfiguration
{
public:
	CanvasDisplayConfiguration()
	{
		displayResolution = OEMakeSize(640, 480);
		displayPixelDensity = OEMakeSize(72, 72),
		displayBarrel = 0;
		displayScanlineAlpha = 0;
		displayCenterLighting = 1;
		displayShadowMask = CANVAS_SHADOWMASK_TRIAD;
		displayShadowMaskDotPitch = 1;
		displayShadowMaskAlpha = 0;
		displayPersistance = 0;
		
		videoDecoder = CANVAS_DECODER_RGB;
		videoBandwidth = 1;
		videoBrightness = 0;
		videoContrast = 1;
		videoSaturation = 1;
		videoHue = 0;
		videoRect = OEMakeRect(0, 0, 1, 1);
		
		compositeBlackLevel = 0;
		compositeWhiteLevel = 1;
		compositeCarrierFrequency = 0.25;
		compositeLinePhase = 0;
		compositeChromaBandwidth = 0.01;
	}
	
	OESize displayResolution;
	OESize displayPixelDensity;
	float displayBarrel;
	float displayScanlineAlpha;
	float displayCenterLighting;
	float displayShadowMaskAlpha;
	float displayShadowMaskDotPitch;
	CanvasShadowMask displayShadowMask;
	float displayPersistance;
	
	CanvasDecoder videoDecoder;
	float videoBandwidth;
	float videoBrightness;
	float videoContrast;
	float videoSaturation;
	float videoHue;
	OERect videoRect;
	
	float compositeBlackLevel;
	float compositeWhiteLevel;
	float compositeCarrierFrequency;
	float compositeLinePhase;
	float compositeChromaBandwidth;
};

// Paper canvas configuration:
// * pageResolution is the number of pixels resolved on a page
// * pagePixelDensity is the number of resolved pixels on a page per inch
// * Note that for paper canvases, pixel density is related to the image sampling frequency

class CanvasPaperConfiguration
{
public:
	CanvasPaperConfiguration()
	{
		pageResolution = OEMakeSize(612, 792);
		pagePixelDensity = OEMakeSize(72, 72);
	}
	
	OESize pageResolution;
	OESize pagePixelDensity;
};

// OpenGL canvas configuration:
// * pageResolution is the number of pixels resolved on a page
// * pagePixelDensity is the number of resolved pixels on a page per inch
// * Note that for paper canvases, pixel density is related to the image sampling frequency

class CanvasOpenGLConfiguration
{
public:
	CanvasOpenGLConfiguration()
	{
		viewportSize = OEMakeSize(612, 792);
		viewportPixelDensity = OEMakeSize(72, 72);
	}
	
	OESize viewportSize;
	OESize viewportPixelDensity;
};

// Canvas HID notifications

typedef struct
{
	int usageId;
	float value;
} CanvasHIDNotification;

#define CANVAS_KEYBOARD_KEY_NUM		256
#define CANVAS_POINTER_BUTTON_NUM	8
#define CANVAS_MOUSE_BUTTON_NUM		8
#define CANVAS_JOYSTICK_NUM			4
#define CANVAS_JOYSTICK_AXIS_NUM	16
#define CANVAS_JOYSTICK_BUTTON_NUM	16
#define CANVAS_JOYSTICK_HAT_NUM		4
#define CANVAS_JOYSTICK_RAXIS_NUM	4

typedef enum
{
	CANVAS_K_A = 0x04,
	CANVAS_K_B,
	CANVAS_K_C,
	CANVAS_K_D,
	CANVAS_K_E,
	CANVAS_K_F,
	CANVAS_K_G,
	CANVAS_K_H,
	CANVAS_K_I,
	CANVAS_K_J,
	CANVAS_K_K,
	CANVAS_K_L,
	CANVAS_K_M,
	CANVAS_K_N,
	CANVAS_K_O,
	CANVAS_K_P,
	CANVAS_K_Q,
	CANVAS_K_R,
	CANVAS_K_S,
	CANVAS_K_T,
	CANVAS_K_U,
	CANVAS_K_V,
	CANVAS_K_W,
	CANVAS_K_X,
	CANVAS_K_Y,
	CANVAS_K_Z,
	CANVAS_K_1,
	CANVAS_K_2,
	CANVAS_K_3,
	CANVAS_K_4,
	CANVAS_K_5,
	CANVAS_K_6,
	CANVAS_K_7,
	CANVAS_K_8,
	CANVAS_K_9,
	CANVAS_K_0,
	CANVAS_K_ENTER,
	CANVAS_K_ESCAPE,
	CANVAS_K_BACKSPACE,
	CANVAS_K_TAB,
	CANVAS_K_SPACE,
	CANVAS_K_MINUS,
	CANVAS_K_EQUAL,
	CANVAS_K_LEFTBRACKET,
	CANVAS_K_RIGHTBRACKET,
	CANVAS_K_BACKSLASH,
	CANVAS_K_NON_US1,
	CANVAS_K_SEMICOLON,
	CANVAS_K_QUOTE,
	CANVAS_K_GRAVEACCENT,
	CANVAS_K_COMMA,
	CANVAS_K_PERIOD,
	CANVAS_K_SLASH,
	CANVAS_K_CAPSLOCK,
	CANVAS_K_F1,
	CANVAS_K_F2,
	CANVAS_K_F3,
	CANVAS_K_F4,
	CANVAS_K_F5,
	CANVAS_K_F6,
	CANVAS_K_F7,
	CANVAS_K_F8,
	CANVAS_K_F9,
	CANVAS_K_F10,
	CANVAS_K_F11,
	CANVAS_K_F12,
	CANVAS_K_PRINTSCREEN,
	CANVAS_K_SCROLLLOCK,
	CANVAS_K_PAUSE,
	CANVAS_K_INSERT,
	CANVAS_K_HOME,
	CANVAS_K_PAGEUP,
	CANVAS_K_DELETE,
	CANVAS_K_END,
	CANVAS_K_PAGEDOWN,
	CANVAS_K_RIGHT,
	CANVAS_K_LEFT,
	CANVAS_K_DOWN,
	CANVAS_K_UP,
	CANVAS_KP_NUMLOCK,
	CANVAS_KP_SLASH,
	CANVAS_KP_STAR,
	CANVAS_KP_MINUS,
	CANVAS_KP_PLUS,
	CANVAS_KP_ENTER,
	CANVAS_KP_1,
	CANVAS_KP_2,
	CANVAS_KP_3,
	CANVAS_KP_4,
	CANVAS_KP_5,
	CANVAS_KP_6,
	CANVAS_KP_7,
	CANVAS_KP_8,
	CANVAS_KP_9,
	CANVAS_KP_0,
	CANVAS_KP_PERIOD,
	CANVAS_K_NON_US2,
	CANVAS_K_APPLICATION,
	CANVAS_K_POWER,
	CANVAS_KP_EQUAL,
	CANVAS_K_F13,
	CANVAS_K_F14,
	CANVAS_K_F15,
	CANVAS_K_F16,
	CANVAS_K_F17,
	CANVAS_K_F18,
	CANVAS_K_F19,
	CANVAS_K_F20,
	CANVAS_K_F21,
	CANVAS_K_F22,
	CANVAS_K_F23,
	CANVAS_K_F24,
	CANVAS_K_EXECUTE,
	CANVAS_K_HELP,
	CANVAS_K_MENU,
	CANVAS_K_SELECT,
	CANVAS_K_STOP,
	CANVAS_K_AGAIN,
	CANVAS_K_UNDO,
	CANVAS_K_CUT,
	CANVAS_K_COPY,
	CANVAS_K_PASTE,
	CANVAS_K_FIND,
	CANVAS_K_MUTE,
	CANVAS_K_VOLUMEUP,
	CANVAS_K_VOLUMEDOWN,
	CANVAS_K_LOCKINGCAPSLOCK,
	CANVAS_K_LOCKINGNUMLOCK,
	CANVAS_K_LOCKINGSCROLLLOCK,
	CANVAS_KP_COMMA,
	CANVAS_KP_EQUAL2,
	CANVAS_K_INTERNATIONAL1,
	CANVAS_K_INTERNATIONAL2,
	CANVAS_K_INTERNATIONAL3,
	CANVAS_K_INTERNATIONAL4,
	CANVAS_K_INTERNATIONAL5,
	CANVAS_K_INTERNATIONAL6,
	CANVAS_K_INTERNATIONAL7,
	CANVAS_K_INTERNATIONAL8,
	CANVAS_K_INTERNATIONAL9,
	CANVAS_K_LANG1,
	CANVAS_K_LANG2,
	CANVAS_K_LANG3,
	CANVAS_K_LANG4,
	CANVAS_K_LANG5,
	CANVAS_K_LANG6,
	CANVAS_K_LANG7,
	CANVAS_K_LANG8,
	CANVAS_K_LANG9,
	CANVAS_K_ALTERASE,
	CANVAS_K_SYSREQ,
	CANVAS_K_CANCEL,
	CANVAS_K_CLEAR,
	CANVAS_K_PRIOR,
	CANVAS_K_RETURN,
	CANVAS_K_SEPARATOR,
	CANVAS_K_OUT,
	CANVAS_K_OPER,
	CANVAS_K_CLEARAGAIN,
	CANVAS_K_CRSELPROPS,
	CANVAS_K_EXSEL,
	
	CANVAS_KP_00 = 0xb0,
	CANVAS_KP_000,
	CANVAS_KP_THOUSANDSSEPARATOR,
	CANVAS_KP_DECIMALSEPARATOR,
	CANVAS_KP_CURRENCYUNIT,
	CANVAS_KP_CURRENCYSUBUNIT,
	CANVAS_KP_LEFTPARENTHESIS,
	CANVAS_KP_RIGHTPARENTHESIS,
	CANVAS_KP_LEFTCURLYBRACKET,
	CANVAS_KP_RIGHTCURLYBRACKET,
	CANVAS_KP_TAB,
	CANVAS_KP_BACKSPACE,
	CANVAS_KP_A,
	CANVAS_KP_B,
	CANVAS_KP_C,
	CANVAS_KP_D,
	CANVAS_KP_E,
	CANVAS_KP_F,
	CANVAS_KP_XOR,
	CANVAS_KP_CARET,
	CANVAS_KP_PERCENT,
	CANVAS_KP_LESS,
	CANVAS_KP_MORE,
	CANVAS_KP_AND,
	CANVAS_KP_LOGICAND,
	CANVAS_KP_OR,
	CANVAS_KP_LOGICOR,
	CANVAS_KP_COLON,
	CANVAS_KP_NUMERAL,
	CANVAS_KP_SPACE,
	CANVAS_KP_AT,
	CANVAS_KP_EXCLAMATION,
	CANVAS_KP_MEMSTORE,
	CANVAS_KP_MEMRECALL,
	CANVAS_KP_MEMCLEAR,
	CANVAS_KP_MEMADD,
	CANVAS_KP_MEMSUBTRACT,
	CANVAS_KP_MEMMULTIPLY,
	CANVAS_KP_MEMDIVIDE,
	CANVAS_KP_PLUSMINUS,
	CANVAS_KP_CLEAR,
	CANVAS_KP_CLEARENTRY,
	CANVAS_KP_BINARY,
	CANVAS_KP_OCTAL,
	CANVAS_KP_DECIMAL,
	CANVAS_KP_HEXADECIMAL,
	
	CANVAS_K_LEFTCONTROL = 0xe0,
	CANVAS_K_LEFTSHIFT,
	CANVAS_K_LEFTALT,
	CANVAS_K_LEFTGUI,
	CANVAS_K_RIGHTCONTROL,
	CANVAS_K_RIGHTSHIFT,
	CANVAS_K_RIGHTALT,
	CANVAS_K_RIGHTGUI,
} CanvasKeyboardUsageIds;

typedef enum
{
	CANVAS_P_X,
	CANVAS_P_Y,
	CANVAS_P_PROXIMITY,
	CANVAS_P_BUTTON1,
	CANVAS_P_BUTTON2,
	CANVAS_P_BUTTON3,
	CANVAS_P_BUTTON4,
	CANVAS_P_BUTTON5,
	CANVAS_P_BUTTON6,
	CANVAS_P_BUTTON7,
	CANVAS_P_BUTTON8,
	CANVAS_P_WHEELX,
	CANVAS_P_WHEELY,
} CanvasPointerUsageIds;

typedef enum
{
	CANVAS_M_RELX,
	CANVAS_M_RELY,
	CANVAS_M_BUTTON1,
	CANVAS_M_BUTTON2,
	CANVAS_M_BUTTON3,
	CANVAS_M_BUTTON4,
	CANVAS_M_BUTTON5,
	CANVAS_M_BUTTON6,
	CANVAS_M_BUTTON7,
	CANVAS_M_BUTTON8,
	CANVAS_M_WHEELX,
	CANVAS_M_WHEELY,
} CanvasMouseUsageIds;

typedef enum
{
	CANVAS_J_AXIS1,
	CANVAS_J_AXIS2,
	CANVAS_J_AXIS3,
	CANVAS_J_AXIS4,
	CANVAS_J_AXIS5,
	CANVAS_J_AXIS6,
	CANVAS_J_AXIS7,
	CANVAS_J_AXIS8,
	CANVAS_J_AXIS9,
	CANVAS_J_AXIS10,
	CANVAS_J_AXIS11,
	CANVAS_J_AXIS12,
	CANVAS_J_AXIS13,
	CANVAS_J_AXIS14,
	CANVAS_J_AXIS15,
	CANVAS_J_AXIS16,
	CANVAS_J_BUTTON1,
	CANVAS_J_BUTTON2,
	CANVAS_J_BUTTON3,
	CANVAS_J_BUTTON4,
	CANVAS_J_BUTTON5,
	CANVAS_J_BUTTON6,
	CANVAS_J_BUTTON7,
	CANVAS_J_BUTTON8,
	CANVAS_J_BUTTON9,
	CANVAS_J_BUTTON10,
	CANVAS_J_BUTTON11,
	CANVAS_J_BUTTON12,
	CANVAS_J_BUTTON13,
	CANVAS_J_BUTTON14,
	CANVAS_J_BUTTON15,
	CANVAS_J_BUTTON16,
	CANVAS_J_HAT1,
	CANVAS_J_HAT2,
	CANVAS_J_HAT3,
	CANVAS_J_HAT4,
	CANVAS_J_RELAXIS1,
	CANVAS_J_RELAXIS2,
	CANVAS_J_RELAXIS3,
	CANVAS_J_RELAXIS4,
} CanvasJoystickUsageIds;

typedef struct
{
	OESize viewportSize;
	bool shouldDraw;
} CanvasVSync;

#endif
