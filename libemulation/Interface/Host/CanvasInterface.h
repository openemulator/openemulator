
/**
 * libemulation
 * Canvas interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the canvas interface
 */

// Notes:
// * setMode sets the canvas mode (CanvasMode):
//   - display (fixed aspect ratio)
//   - paper (large scrolling areas)
//   - OpenGL (custom graphics)
// * setCaptureMode describes how mouse and keyboard are captured (CanvasCaptureMode)
// * setKeyboardFlags sets the keyboard LEDs (CanvasKeyboardFlags)
// * setBezel sets the canvas' bezel (CanvasBezel)

// Configuration:
// * configureDisplay configures a display canvas (CanvasDisplayConfiguration)
// * configurePaper configures a paper canvas (CanvasPaperConfiguration)
// * configureOpenGL configures an OpenGL canvas (CanvasOpenGLConfiguration)

// Drawing:
// * postImage post an image to the canvas (OEImage)
// * clear clears the canvas
// * movePrintHead sets the print head position in paper mode (OEPoint)

// Notifications:
// * HID notifications use the CanvasHIDNotification structure
// * Pointer coordinates are in [0..1, 0..1] range (origin is top left)
// * didCopy, didPaste is sent when the user requests copy/paste (wstring)
// * didDelete is sent when the user requests delete
// * didVSync is called from the video thread after vertical sync (CanvasVSync)
//   It should be used for posting images or for requesting redraw
// * willDraw is called from the video thread when an OpenGL canvas will be
//   drawn (OESize)
// * HID values are in the [0..1] range

// Multithreading, beware!
// * didVSync and willDraw are sent from the drawing thread! Keep this in mind
//   for managing threads correctly.

#ifndef _CANVASINTERFACE_H
#define _CANVASINTERFACE_H

#include "OEImage.h"

#define NTSC_FSC        (315.0 / 88.0 * 1E6)
#define NTSC_4FSC       (4 * NTSC_FSC)
#define NTSC_HTOTAL     ((63.0 + 5.0 / 9.0) * 1E-6)
#define NTSC_HLENGTH    ((52.0 + 8.0 / 9.0) * 1E-6)
#define NTSC_HHALF      ((35.0 + 2.0 / 3.0) * 1E-6)
#define NTSC_HSTART     (NTSC_HHALF - NTSC_HLENGTH / 2.0)
#define NTSC_HEND       (NTSC_HHALF + NTSC_HLENGTH / 2.0)
#define NTSC_VTOTAL     262
#define NTSC_VLENGTH    240
#define NTSC_VSTART     19
#define NTSC_VEND       (NTSC_VSTART + NTSC_VLENGTH)

#define PAL_FSC         4433618.75
#define PAL_4FSC        (4 * PAL_FSC)
#define PAL_HTOTAL      64.0E-6
#define PAL_HLENGTH     52.0E-6
#define PAL_HHALF       ((37.0 + 10.0 / 27.0) * 1E-6)
#define PAL_HSTART      (PAL_HHALF - PAL_HLENGTH / 2.0)
#define PAL_HEND        (PAL_HHALF + PAL_HLENGTH / 2.0)
#define PAL_VTOTAL      312
#define PAL_VLENGTH     288
#define PAL_VSTART      21
#define PAL_VEND        (PAL_VSTART + PAL_VLENGTH)

typedef enum
{
    CANVAS_SET_MODE,
    CANVAS_SET_CAPTUREMODE,
    CANVAS_SET_BEZEL,
    
    CANVAS_SET_KEYBOARD_LEDS,
    CANVAS_GET_KEYBOARD_FLAGS,
    CANVAS_GET_KEYBOARD_ANYKEYDOWN,
    
    CANVAS_CONFIGURE_DISPLAY,
    CANVAS_CONFIGURE_PAPER,
    CANVAS_CONFIGURE_OPENGL,
    
    CANVAS_POST_IMAGE,
    CANVAS_CLEAR,
    CANVAS_MOVE_PRINTHEAD,
    
    CANVAS_END,
} CanvasMessage;

typedef enum
{
    CANVAS_UNICODECHAR_WAS_SENT,
    CANVAS_KEYBOARD_DID_CHANGE,
    CANVAS_POINTER_DID_CHANGE,
    CANVAS_MOUSE_DID_CHANGE,
    
    CANVAS_DID_COPY,
    CANVAS_DID_PASTE,
    CANVAS_DID_DELETE,
    
    CANVAS_DID_VSYNC,
    CANVAS_WILL_DRAW,
    
    CANVAS_NOTIFICATION_END,
} CanvasNotification;

// Definitions

typedef enum
{
    CANVAS_DISPLAY,
    CANVAS_PAPER,
    CANVAS_OPENGL,
} CanvasMode;

typedef enum
{
    CANVAS_NO_CAPTURE,
    CANVAS_CAPTURE_ON_MOUSE_CLICK,
    CANVAS_CAPTURE_ON_MOUSE_ENTER,
} CanvasCaptureMode;

typedef enum
{
    CANVAS_BEZEL_NONE,
    CANVAS_BEZEL_POWER,
    CANVAS_BEZEL_PAUSE,
} CanvasBezel;

// Display canvas configuration:
// * displayResolution is the number of resolved pixels of the device (this
//   is not necessarily the video frame resolution)
// * displayPixelDensity is the number of resolved pixels of the device per inch
// * The shadow mask dot pitch measures the spacing of the pixels in mm
// * Video bandwidths are in Hz
// * Persistence is in frames

typedef enum
{
    CANVAS_RGB,
    CANVAS_MONOCHROME,
    CANVAS_YUV,
    CANVAS_YIQ,
    CANVAS_CXA2025AS,
} CanvasDecoder;

typedef enum
{
    CANVAS_TRIAD,
    CANVAS_INLINE,
    CANVAS_APERTURE,
    CANVAS_LCD,
    CANVAS_BAYER,
} CanvasShadowMask;

class CanvasDisplayConfiguration
{
public:
    CanvasDisplayConfiguration()
    {
        videoDecoder = CANVAS_RGB;
        videoBrightness = 0;
        videoContrast = 1;
        videoSaturation = 1;
        videoHue = 0;
        videoCenter = OEMakePoint(0, 0);
        videoSize = OEMakeSize(1, 1);
        videoBandwidth = 14318180.0;
        videoLumaBandwidth = 600000.0;
        videoChromaBandwidth = 2000000.0;
        videoWhiteOnly = false;
        
        displayResolution = OEMakeSize(640, 480);
        displayPixelDensity = 72;
        displayBarrel = 0;
        displayScanlineLevel = 0;
        displayShadowMaskLevel = 0;
        displayShadowMaskDotPitch = 1;
        displayShadowMask = CANVAS_TRIAD;
        displayPersistence = 0;
        displayCenterLighting = 1;
        displayLuminanceGain = 1;
    }
    
    CanvasDecoder videoDecoder;
    float videoBrightness;
    float videoContrast;
    float videoSaturation;
    float videoHue;
    OEPoint videoCenter;
    OESize videoSize;
    float videoBandwidth;
    float videoLumaBandwidth;
    float videoChromaBandwidth;
    bool videoWhiteOnly;
    
    OESize displayResolution;
    float displayPixelDensity;
    float displayBarrel;
    float displayScanlineLevel;
    float displayShadowMaskLevel;
    float displayShadowMaskDotPitch;
    CanvasShadowMask displayShadowMask;
    float displayPersistence;
    float displayCenterLighting;
    float displayLuminanceGain;
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
// * viewportDefaultSize is the default canvas size in display pixels
// * viewportPixelDensity is scaling when printing

class CanvasOpenGLConfiguration
{
public:
    CanvasOpenGLConfiguration()
    {
        viewportDefaultSize = OEMakeSize(640, 480);
        viewportPixelDensity = 72;
    }
    
    OESize viewportDefaultSize;
    float viewportPixelDensity;
};

// Canvas HID notification events

typedef struct
{
    OEInt usageId;
    float value;
} CanvasHIDEvent;

// Canvas keyboard events

typedef OEInt CanvasUnicodeChar;

typedef enum
{
    CANVAS_U_UP = 0xf700,
    CANVAS_U_DOWN,
    CANVAS_U_LEFT,
    CANVAS_U_RIGHT,
    CANVAS_U_F1,
    CANVAS_U_F2,
    CANVAS_U_F3,
    CANVAS_U_F4,
    CANVAS_U_F5,
    CANVAS_U_F6,
    CANVAS_U_F7,
    CANVAS_U_F8,
    CANVAS_U_F9,
    CANVAS_U_F10,
    CANVAS_U_F11,
    CANVAS_U_F12,
    CANVAS_U_F13,
    CANVAS_U_F14,
    CANVAS_U_F15,
    CANVAS_U_F16,
    CANVAS_U_F17,
    CANVAS_U_F18,
    CANVAS_U_F19,
    CANVAS_U_F20,
    CANVAS_U_F21,
    CANVAS_U_F22,
    CANVAS_U_F23,
    CANVAS_U_F24,
    CANVAS_U_F25,
    CANVAS_U_F26,
    CANVAS_U_F27,
    CANVAS_U_F28,
    CANVAS_U_F29,
    CANVAS_U_F30,
    CANVAS_U_F31,
    CANVAS_U_F32,
    CANVAS_U_F33,
    CANVAS_U_F34,
    CANVAS_U_F35,
    CANVAS_U_INSERT,
    CANVAS_U_DELETE,
    CANVAS_U_HOME,
    CANVAS_U_BEGIN,
    CANVAS_U_END,
    CANVAS_U_PAGEUP,
    CANVAS_U_PAGEDOWN,
    CANVAS_U_PRINTSCREEN,
    CANVAS_U_SCROLLLOCK,
    CANVAS_U_PAUSE,
    CANVAS_U_SYSREQ,
    CANVAS_U_BREAK,
    CANVAS_U_RESET,
    CANVAS_U_STOP,
    CANVAS_U_MENU,
    CANVAS_U_USER,
    CANVAS_U_SYSTEM,
    CANVAS_U_PRINT,
    CANVAS_U_CLEARLINE,
    CANVAS_U_CLEARDISPLAY,
    CANVAS_U_INSERTLINE,
    CANVAS_U_DELETELINE,
    CANVAS_U_INSERTCHAR,
    CANVAS_U_DELETECHAR,
    CANVAS_U_PREV,
    CANVAS_U_NEXT,
    CANVAS_U_SELECT,
    CANVAS_U_EXECUTE,
    CANVAS_U_UNDO,
    CANVAS_U_REDO,
    CANVAS_U_FIND,
    CANVAS_U_HELP,
    CANVAS_U_MODESWITCH,
} CanvasUnicodeFunctionKey;

#define CANVAS_KEYBOARD_KEY_NUM		256

typedef OEInt CanvasKeyboardLEDs;

#define CANVAS_KL_NUMLOCK       (1 << 0)
#define CANVAS_KL_CAPSLOCK      (1 << 1)
#define CANVAS_KL_SCROLLLOCK	(1 << 2)
#define CANVAS_KL_COMPOSE       (1 << 3)
#define CANVAS_KL_KANA          (1 << 4)
#define CANVAS_KL_POWER         (1 << 5)
#define CANVAS_KL_SHIFT         (1 << 6)

typedef OEInt CanvasKeyboardFlags;

#define CANVAS_KF_LEFTCONTROL	(1 << 0)
#define CANVAS_KF_LEFTSHIFT     (1 << 1)
#define CANVAS_KF_LEFTALT       (1 << 2)
#define CANVAS_KF_LEFTGUI       (1 << 3)
#define CANVAS_KF_RIGHTCONTROL	(1 << 4)
#define CANVAS_KF_RIGHTSHIFT	(1 << 5)
#define CANVAS_KF_RIGHTALT		(1 << 6)
#define CANVAS_KF_RIGHTGUI		(1 << 7)

#define CANVAS_KF_CONTROL       (CANVAS_KF_LEFTCONTROL | CANVAS_KF_RIGHTCONTROL)
#define CANVAS_KF_SHIFT         (CANVAS_KF_LEFTSHIFT | CANVAS_KF_RIGHTSHIFT)
#define CANVAS_KF_ALT           (CANVAS_KF_LEFTALT | CANVAS_KF_RIGHTALT)
#define CANVAS_KF_GUI           (CANVAS_KF_LEFTGUI | CANVAS_KF_RIGHTGUI)

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
} CanvasKeyboardUsageId;

// Canvas pointer events

#define CANVAS_POINTER_BUTTON_NUM	8

typedef enum
{
    CANVAS_P_PROXIMITY,
    CANVAS_P_X,
    CANVAS_P_Y,
    CANVAS_P_WHEELX,
    CANVAS_P_WHEELY,
    CANVAS_P_BUTTON1,
    CANVAS_P_BUTTON2,
    CANVAS_P_BUTTON3,
    CANVAS_P_BUTTON4,
    CANVAS_P_BUTTON5,
    CANVAS_P_BUTTON6,
    CANVAS_P_BUTTON7,
    CANVAS_P_BUTTON8,
} CanvasPointerUsageId;

// Canvas mouse events

#define CANVAS_MOUSE_BUTTON_NUM		8

typedef enum
{
    CANVAS_M_RX,
    CANVAS_M_RY,
    CANVAS_M_WHEELX,
    CANVAS_M_WHEELY,
    CANVAS_M_BUTTON1,
    CANVAS_M_BUTTON2,
    CANVAS_M_BUTTON3,
    CANVAS_M_BUTTON4,
    CANVAS_M_BUTTON5,
    CANVAS_M_BUTTON6,
    CANVAS_M_BUTTON7,
    CANVAS_M_BUTTON8,
} CanvasMouseUsageId;

// Canvas vsync events

typedef struct
{
    OESize viewportSize;
    bool shouldDraw;
} CanvasVSync;

#endif
