
/**
 * libemulator
 * Host Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host ports (serial, parallel, ethernet, usb, firewire)
 */

#include "OEComponent.h"

#define HID_PAGE_GENERIC					0x01
#define HID_PAGE_KEYBOARD					0x07
#define HID_PAGE_LED						0x08

#define HID_S_POWERDOWN						0x81
#define HID_S_SLEEP							0x82
#define HID_S_WAKEUP						0x83
#define HID_S_CONTEXT						0x84
#define HID_S_MAINMENU						0x85
#define HID_S_APPMENU						0x86
#define HID_S_MENUHELP						0x87
#define HID_S_MENUEXIT						0x88
#define HID_S_MENUSELECT					0x89
#define HID_S_MENURIGHT						0x8a
#define HID_S_MENULEFT						0x8b
#define HID_S_MENUUP						0x8c
#define HID_S_MENUDOWN						0x8d
#define HID_S_COLDRESTART					0x8d
#define HID_S_WARMRESTART					0x8f
#define HID_S_DPADUP						0x90
#define HID_S_DPADDOWN						0x91
#define HID_S_DPADRIGHT						0x92
#define HID_S_DPADLEFT						0x93

#define HID_S_DOCK							0xa0
#define HID_S_UNDOCK						0xa1
#define HID_S_SETUP							0xa2
#define HID_S_BREAK							0xa3
#define HID_S_DEBUGGERBREAK					0xa4
#define HID_S_APPLICATIONBREAK				0xa5
#define HID_S_APPLICATIONDEBUGGERBREAK		0xa6
#define HID_S_SPEAKERMUTE					0xa7
#define HID_S_HIBERNATE						0xa8

#define HID_S_DISPLAYINVERT					0xb0
#define HID_S_DISPLAYINTERNAL				0xb1
#define HID_S_DISPLAYEXTERNAL				0xb2
#define HID_S_DISPLAYBOTH					0xb3
#define HID_S_DISPLAYDUAL					0xb4
#define HID_S_DISPLAYTOGGLEINTEXT			0xb5
#define HID_S_DISPLAYSWAPPRIMARYSECONDARY	0xb6
#define HID_S_DISPLAYLCDAUTOSCALE			0xb7

#define HID_K_A								0x04
#define HID_K_B								0x05
#define HID_K_C								0x06
#define HID_K_D								0x07
#define HID_K_E								0x08
#define HID_K_F								0x09
#define HID_K_G								0x0a
#define HID_K_H								0x0b
#define HID_K_I								0x0c
#define HID_K_J								0x0d
#define HID_K_K								0x0e
#define HID_K_L								0x0f
#define HID_K_M								0x10
#define HID_K_N								0x11
#define HID_K_O								0x12
#define HID_K_P								0x13
#define HID_K_Q								0x14
#define HID_K_R								0x15
#define HID_K_S								0x16
#define HID_K_T								0x17
#define HID_K_U								0x18
#define HID_K_V								0x19
#define HID_K_W								0x1a
#define HID_K_X								0x1b
#define HID_K_Y								0x1c
#define HID_K_Z								0x1d
#define HID_K_1								0x1e
#define HID_K_2								0x1f
#define HID_K_3								0x20
#define HID_K_4								0x21
#define HID_K_5								0x22
#define HID_K_6								0x23
#define HID_K_7								0x24
#define HID_K_8								0x25
#define HID_K_9								0x26
#define HID_K_0								0x27
#define HID_K_ENTER							0x28
#define HID_K_ESCAPE						0x29
#define HID_K_BACKSPACE						0x2a
#define HID_K_TAB							0x2b
#define HID_K_SPACE							0x2c
#define HID_K_MINUS							0x2d
#define HID_K_EQUAL							0x2e
#define HID_K_LEFTBRACKET					0x2f
#define HID_K_RIGHTBRACKET					0x30
#define HID_K_BACKSLASH						0x31
#define HID_K_NON_US1						0x32
#define HID_K_SEMICOLON						0x33
#define HID_K_QUOTE							0x34
#define HID_K_GRAVEACCENT					0x35
#define HID_K_COMMA							0x36
#define HID_K_PERIOD						0x37
#define HID_K_SLASH							0x38
#define HID_K_CAPSLOCK						0x39
#define HID_K_F1							0x3a
#define HID_K_F2							0x3b
#define HID_K_F3							0x3c
#define HID_K_F4							0x3d
#define HID_K_F5							0x3e
#define HID_K_F6							0x3f
#define HID_K_F7							0x40
#define HID_K_F8							0x41
#define HID_K_F9							0x42
#define HID_K_F10							0x43
#define HID_K_F11							0x44
#define HID_K_F12							0x45
#define HID_K_PRINTSCREEN					0x46
#define HID_K_SCROLLLOCK					0x47
#define HID_K_PAUSE							0x48
#define HID_K_INSERT						0x49
#define HID_K_HOME							0x4a
#define HID_K_PAGEUP						0x4b
#define HID_K_DELETE						0x4c
#define HID_K_END							0x4d
#define HID_K_PAGEDOWN						0x4e
#define HID_K_RIGHT							0x4f
#define HID_K_LEFT							0x50
#define HID_K_DOWN							0x51
#define HID_K_UP							0x52
#define HID_KP_NUMLOCK						0x53
#define HID_KP_SLASH						0x54
#define HID_KP_STAR							0x55
#define HID_KP_MINUS						0x56
#define HID_KP_PLUS							0x57
#define HID_KP_ENTER						0x58
#define HID_KP_1							0x59
#define HID_KP_2							0x5a
#define HID_KP_3							0x5b
#define HID_KP_4							0x5c
#define HID_KP_5							0x5d
#define HID_KP_6							0x5e
#define HID_KP_7							0x5f
#define HID_KP_8							0x60
#define HID_KP_9							0x61
#define HID_KP_0							0x62
#define HID_KP_PERIOD						0x63
#define HID_KP_EQUAL						0x67
#define HID_K_NON_US2						0x64
#define HID_K_APPLICATION					0x65
#define HID_K_POWER							0x66
#define HID_K_F13							0x68
#define HID_K_F14							0x69
#define HID_K_F15							0x6a
#define HID_K_F16							0x6b
#define HID_K_F17							0x6c
#define HID_K_F18							0x6d
#define HID_K_F19							0x6e
#define HID_K_F20							0x6f
#define HID_K_F21							0x70
#define HID_K_F22							0x71
#define HID_K_F23							0x72
#define HID_K_F24							0x73
#define HID_K_EXECUTE						0x74
#define HID_K_HELP							0x75
#define HID_K_MENU							0x76
#define HID_K_SELECT						0x77
#define HID_K_STOP							0x78
#define HID_K_AGAIN							0x79
#define HID_K_UNDO							0x7a
#define HID_K_CUT							0x7b
#define HID_K_COPY							0x7c
#define HID_K_PASTE							0x7d
#define HID_K_FIND							0x7e
#define HID_K_MUTE							0x7f
#define HID_K_VOLUMEUP						0x80
#define HID_K_VOLUMEDOWN					0x81
#define HID_K_LOCKINGCAPSLOCK				0x82
#define HID_K_LOCKINGNUMLOCK				0x83
#define HID_K_LOCKINGSCROLLLOCK				0x84
#define HID_KP_COMMA						0x85
#define HID_KP_EQUAL2						0x86
#define HID_K_INTERNATIONAL1				0x87
#define HID_K_INTERNATIONAL2				0x88
#define HID_K_INTERNATIONAL3				0x89
#define HID_K_INTERNATIONAL4				0x8a
#define HID_K_INTERNATIONAL5				0x8b
#define HID_K_INTERNATIONAL6				0x8c
#define HID_K_INTERNATIONAL7				0x8d
#define HID_K_INTERNATIONAL8				0x8e
#define HID_K_INTERNATIONAL9				0x8f
#define HID_K_LANG1							0x90
#define HID_K_LANG2							0x91
#define HID_K_LANG3							0x92
#define HID_K_LANG4							0x93
#define HID_K_LANG5							0x94
#define HID_K_LANG6							0x95
#define HID_K_LANG7							0x96
#define HID_K_LANG8							0x97
#define HID_K_LANG9							0x98
#define HID_K_ALTERASE						0x99
#define HID_K_SYSREQ						0x9a
#define HID_K_CANCEL						0x9b
#define HID_K_CLEAR							0x9c
#define HID_K_PRIOR							0x9d
#define HID_K_RETURN						0x9e
#define HID_K_SEPARATOR						0x9f
#define HID_K_OUT							0xa0
#define HID_K_OPER							0xa1
#define HID_K_CLEARAGAIN					0xa2
#define HID_K_CRSELPROPS					0xa3
#define HID_K_EXSEL							0xa4

#define HID_KP_00							0xb0
#define HID_KP_000							0xb1
#define HID_KP_THOUSANDSSEPARATOR			0xb2
#define HID_KP_DECIMALSEPARATOR				0xb3
#define HID_KP_CURRENCYUNIT					0xb4
#define HID_KP_CURRENCYSUBUNIT				0xb5
#define HID_KP_LEFTPARENTHESIS				0xb6
#define HID_KP_RIGHTPARENTHESIS				0xb7
#define HID_KP_LEFTCURLYBRACKET				0xb8
#define HID_KP_RIGHTCURLYBRACKET			0xb9
#define HID_KP_TAB							0xba
#define HID_KP_BACKSPACE					0xbb
#define HID_KP_A							0xbc
#define HID_KP_B							0xbd
#define HID_KP_C							0xbe
#define HID_KP_D							0xbf
#define HID_KP_E							0xc0
#define HID_KP_F							0xc1
#define HID_KP_XOR							0xc2
#define HID_KP_CARET						0xc3
#define HID_KP_PERCENT						0xc4
#define HID_KP_LESS							0xc5
#define HID_KP_MORE							0xc6
#define HID_KP_AND							0xc7
#define HID_KP_LOGICAND						0xc8
#define HID_KP_OR							0xc9
#define HID_KP_LOGICOR						0xca
#define HID_KP_COLON						0xcb
#define HID_KP_NUMERAL						0xcc
#define HID_KP_SPACE						0xcd
#define HID_KP_AT							0xce
#define HID_KP_EXCLAMATION					0xcf
#define HID_KP_MEMSTORE						0xd0
#define HID_KP_MEMRECALL					0xd1
#define HID_KP_MEMCLEAR						0xd2
#define HID_KP_MEMADD						0xd3
#define HID_KP_MEMSUBTRACT					0xd4
#define HID_KP_MEMMULTIPLY					0xd5
#define HID_KP_MEMDIVIDE					0xd6
#define HID_KP_PLUSMINUS					0xd7
#define HID_KP_CLEAR						0xd8
#define HID_KP_CLEARENTRY					0xd9
#define HID_KP_BINARY						0xda
#define HID_KP_OCTAL						0xdb
#define HID_KP_DECIMAL						0xdc
#define HID_KP_HEXADECIMAL					0xdd

#define HID_K_LEFTCONTROL					0xe0
#define HID_K_LEFTSHIFT						0xe1
#define HID_K_LEFTALT						0xe2
#define HID_K_LEFTGUI						0xe3
#define HID_K_RIGHTCONTROL					0xe4
#define	HID_K_RIGHTSHIFT					0xe5
#define HID_K_RIGHTALT						0xe6
#define HID_K_RIGHTGUI						0xe7

#define HID_L_NUMLOCK						0x01
#define HID_L_CAPSLOCK						0x02
#define HID_L_SCROLLLOCK					0x03
#define HID_L_COMPOSE						0x04
#define HID_L_KANA							0x05
#define HID_L_SHIFT							0x07

enum
{
	HOSTHID_EVENT = OE_USER,
};

typedef struct
{
	int usagePage;
	int usageId;
	bool isDown;
	int unicode;
	int modifierState;
} OEHIDEvent;

class HostHID : public OEComponent
{
};
