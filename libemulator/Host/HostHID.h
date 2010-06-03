
/**
 * libemulator
 * Host HID
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host HID (system, keyboard, mouse, joystick, tablet)
 */

#include "OEComponent.h"

// Messages
enum
{
	HOSTHID_REGISTER_HOST,
};

// Notifications
enum
{
	HOSTHID_SYSTEM_EVENT,
	HOSTHID_KEYBOARD_EVENT,
	HOSTHID_UNICODEKEYBOARD_EVENT,
	HOSTHID_LED_EVENT,
	HOSTHID_MOUSE_EVENT,
	HOSTHID_JOYSTICK_EVENT,
	HOSTHID_TABLET_EVENT,
};

#define HOSTHID_S_POWERDOWN						0x81
#define HOSTHID_S_SLEEP							0x82
#define HOSTHID_S_WAKEUP						0x83
#define HOSTHID_S_CONTEXT						0x84
#define HOSTHID_S_MAINMENU						0x85
#define HOSTHID_S_APPMENU						0x86
#define HOSTHID_S_MENUHELP						0x87
#define HOSTHID_S_MENUEXIT						0x88
#define HOSTHID_S_MENUSELECT					0x89
#define HOSTHID_S_MENURIGHT						0x8a
#define HOSTHID_S_MENULEFT						0x8b
#define HOSTHID_S_MENUUP						0x8c
#define HOSTHID_S_MENUDOWN						0x8d
#define HOSTHID_S_COLDRESTART					0x8e
#define HOSTHID_S_WARMRESTART					0x8f
#define HOSTHID_S_DPADUP						0x90
#define HOSTHID_S_DPADDOWN						0x91
#define HOSTHID_S_DPADRIGHT						0x92
#define HOSTHID_S_DPADLEFT						0x93
#define HOSTHID_S_DOCK							0xa0
#define HOSTHID_S_UNDOCK						0xa1
#define HOSTHID_S_SETUP							0xa2
#define HOSTHID_S_BREAK							0xa3
#define HOSTHID_S_DEBUGGERBREAK					0xa4
#define HOSTHID_S_APPLICATIONBREAK				0xa5
#define HOSTHID_S_APPLICATIONDEBUGGERBREAK		0xa6
#define HOSTHID_S_SPEAKERMUTE					0xa7
#define HOSTHID_S_HIBERNATE						0xa8
#define HOSTHID_S_DISPLAYINVERT					0xb0
#define HOSTHID_S_DISPLAYINTERNAL				0xb1
#define HOSTHID_S_DISPLAYEXTERNAL				0xb2
#define HOSTHID_S_DISPLAYBOTH					0xb3
#define HOSTHID_S_DISPLAYDUAL					0xb4
#define HOSTHID_S_DISPLAYTOGGLEINTEXT			0xb5
#define HOSTHID_S_DISPLAYSWAPPRIMARYSECONDARY	0xb6
#define HOSTHID_S_DISPLAYLCDAUTOSCALE			0xb7

#define HOSTHID_K_A								0x04
#define HOSTHID_K_B								0x05
#define HOSTHID_K_C								0x06
#define HOSTHID_K_D								0x07
#define HOSTHID_K_E								0x08
#define HOSTHID_K_F								0x09
#define HOSTHID_K_G								0x0a
#define HOSTHID_K_H								0x0b
#define HOSTHID_K_I								0x0c
#define HOSTHID_K_J								0x0d
#define HOSTHID_K_K								0x0e
#define HOSTHID_K_L								0x0f
#define HOSTHID_K_M								0x10
#define HOSTHID_K_N								0x11
#define HOSTHID_K_O								0x12
#define HOSTHID_K_P								0x13
#define HOSTHID_K_Q								0x14
#define HOSTHID_K_R								0x15
#define HOSTHID_K_S								0x16
#define HOSTHID_K_T								0x17
#define HOSTHID_K_U								0x18
#define HOSTHID_K_V								0x19
#define HOSTHID_K_W								0x1a
#define HOSTHID_K_X								0x1b
#define HOSTHID_K_Y								0x1c
#define HOSTHID_K_Z								0x1d
#define HOSTHID_K_1								0x1e
#define HOSTHID_K_2								0x1f
#define HOSTHID_K_3								0x20
#define HOSTHID_K_4								0x21
#define HOSTHID_K_5								0x22
#define HOSTHID_K_6								0x23
#define HOSTHID_K_7								0x24
#define HOSTHID_K_8								0x25
#define HOSTHID_K_9								0x26
#define HOSTHID_K_0								0x27
#define HOSTHID_K_ENTER							0x28
#define HOSTHID_K_ESCAPE						0x29
#define HOSTHID_K_BACKSPACE						0x2a
#define HOSTHID_K_TAB							0x2b
#define HOSTHID_K_SPACE							0x2c
#define HOSTHID_K_MINUS							0x2d
#define HOSTHID_K_EQUAL							0x2e
#define HOSTHID_K_LEFTBRACKET					0x2f
#define HOSTHID_K_RIGHTBRACKET					0x30
#define HOSTHID_K_BACKSLASH						0x31
#define HOSTHID_K_NON_US1						0x32
#define HOSTHID_K_SEMICOLON						0x33
#define HOSTHID_K_QUOTE							0x34
#define HOSTHID_K_GRAVEACCENT					0x35
#define HOSTHID_K_COMMA							0x36
#define HOSTHID_K_PERIOD						0x37
#define HOSTHID_K_SLASH							0x38
#define HOSTHID_K_CAPSLOCK						0x39
#define HOSTHID_K_F1							0x3a
#define HOSTHID_K_F2							0x3b
#define HOSTHID_K_F3							0x3c
#define HOSTHID_K_F4							0x3d
#define HOSTHID_K_F5							0x3e
#define HOSTHID_K_F6							0x3f
#define HOSTHID_K_F7							0x40
#define HOSTHID_K_F8							0x41
#define HOSTHID_K_F9							0x42
#define HOSTHID_K_F10							0x43
#define HOSTHID_K_F11							0x44
#define HOSTHID_K_F12							0x45
#define HOSTHID_K_PRINTSCREEN					0x46
#define HOSTHID_K_SCROLLLOCK					0x47
#define HOSTHID_K_PAUSE							0x48
#define HOSTHID_K_INSERT						0x49
#define HOSTHID_K_HOME							0x4a
#define HOSTHID_K_PAGEUP						0x4b
#define HOSTHID_K_DELETE						0x4c
#define HOSTHID_K_END							0x4d
#define HOSTHID_K_PAGEDOWN						0x4e
#define HOSTHID_K_RIGHT							0x4f
#define HOSTHID_K_LEFT							0x50
#define HOSTHID_K_DOWN							0x51
#define HOSTHID_K_UP							0x52
#define HOSTHID_KP_NUMLOCK						0x53
#define HOSTHID_KP_SLASH						0x54
#define HOSTHID_KP_STAR							0x55
#define HOSTHID_KP_MINUS						0x56
#define HOSTHID_KP_PLUS							0x57
#define HOSTHID_KP_ENTER						0x58
#define HOSTHID_KP_1							0x59
#define HOSTHID_KP_2							0x5a
#define HOSTHID_KP_3							0x5b
#define HOSTHID_KP_4							0x5c
#define HOSTHID_KP_5							0x5d
#define HOSTHID_KP_6							0x5e
#define HOSTHID_KP_7							0x5f
#define HOSTHID_KP_8							0x60
#define HOSTHID_KP_9							0x61
#define HOSTHID_KP_0							0x62
#define HOSTHID_KP_PERIOD						0x63
#define HOSTHID_K_NON_US2						0x64
#define HOSTHID_K_APPLICATION					0x65
#define HOSTHID_K_POWER							0x66
#define HOSTHID_KP_EQUAL						0x67
#define HOSTHID_K_F13							0x68
#define HOSTHID_K_F14							0x69
#define HOSTHID_K_F15							0x6a
#define HOSTHID_K_F16							0x6b
#define HOSTHID_K_F17							0x6c
#define HOSTHID_K_F18							0x6d
#define HOSTHID_K_F19							0x6e
#define HOSTHID_K_F20							0x6f
#define HOSTHID_K_F21							0x70
#define HOSTHID_K_F22							0x71
#define HOSTHID_K_F23							0x72
#define HOSTHID_K_F24							0x73
#define HOSTHID_K_EXECUTE						0x74
#define HOSTHID_K_HELP							0x75
#define HOSTHID_K_MENU							0x76
#define HOSTHID_K_SELECT						0x77
#define HOSTHID_K_STOP							0x78
#define HOSTHID_K_AGAIN							0x79
#define HOSTHID_K_UNDO							0x7a
#define HOSTHID_K_CUT							0x7b
#define HOSTHID_K_COPY							0x7c
#define HOSTHID_K_PASTE							0x7d
#define HOSTHID_K_FIND							0x7e
#define HOSTHID_K_MUTE							0x7f
#define HOSTHID_K_VOLUMEUP						0x80
#define HOSTHID_K_VOLUMEDOWN					0x81
#define HOSTHID_K_LOCKINGCAPSLOCK				0x82
#define HOSTHID_K_LOCKINGNUMLOCK				0x83
#define HOSTHID_K_LOCKINGSCROLLLOCK				0x84
#define HOSTHID_KP_COMMA						0x85
#define HOSTHID_KP_EQUAL2						0x86
#define HOSTHID_K_INTERNATIONAL1				0x87
#define HOSTHID_K_INTERNATIONAL2				0x88
#define HOSTHID_K_INTERNATIONAL3				0x89
#define HOSTHID_K_INTERNATIONAL4				0x8a
#define HOSTHID_K_INTERNATIONAL5				0x8b
#define HOSTHID_K_INTERNATIONAL6				0x8c
#define HOSTHID_K_INTERNATIONAL7				0x8d
#define HOSTHID_K_INTERNATIONAL8				0x8e
#define HOSTHID_K_INTERNATIONAL9				0x8f
#define HOSTHID_K_LANG1							0x90
#define HOSTHID_K_LANG2							0x91
#define HOSTHID_K_LANG3							0x92
#define HOSTHID_K_LANG4							0x93
#define HOSTHID_K_LANG5							0x94
#define HOSTHID_K_LANG6							0x95
#define HOSTHID_K_LANG7							0x96
#define HOSTHID_K_LANG8							0x97
#define HOSTHID_K_LANG9							0x98
#define HOSTHID_K_ALTERASE						0x99
#define HOSTHID_K_SYSREQ						0x9a
#define HOSTHID_K_CANCEL						0x9b
#define HOSTHID_K_CLEAR							0x9c
#define HOSTHID_K_PRIOR							0x9d
#define HOSTHID_K_RETURN						0x9e
#define HOSTHID_K_SEPARATOR						0x9f
#define HOSTHID_K_OUT							0xa0
#define HOSTHID_K_OPER							0xa1
#define HOSTHID_K_CLEARAGAIN					0xa2
#define HOSTHID_K_CRSELPROPS					0xa3
#define HOSTHID_K_EXSEL							0xa4
#define HOSTHID_KP_00							0xb0
#define HOSTHID_KP_000							0xb1
#define HOSTHID_KP_THOUSANDSSEPARATOR			0xb2
#define HOSTHID_KP_DECIMALSEPARATOR				0xb3
#define HOSTHID_KP_CURRENCYUNIT					0xb4
#define HOSTHID_KP_CURRENCYSUBUNIT				0xb5
#define HOSTHID_KP_LEFTPARENTHESIS				0xb6
#define HOSTHID_KP_RIGHTPARENTHESIS				0xb7
#define HOSTHID_KP_LEFTCURLYBRACKET				0xb8
#define HOSTHID_KP_RIGHTCURLYBRACKET			0xb9
#define HOSTHID_KP_TAB							0xba
#define HOSTHID_KP_BACKSPACE					0xbb
#define HOSTHID_KP_A							0xbc
#define HOSTHID_KP_B							0xbd
#define HOSTHID_KP_C							0xbe
#define HOSTHID_KP_D							0xbf
#define HOSTHID_KP_E							0xc0
#define HOSTHID_KP_F							0xc1
#define HOSTHID_KP_XOR							0xc2
#define HOSTHID_KP_CARET						0xc3
#define HOSTHID_KP_PERCENT						0xc4
#define HOSTHID_KP_LESS							0xc5
#define HOSTHID_KP_MORE							0xc6
#define HOSTHID_KP_AND							0xc7
#define HOSTHID_KP_LOGICAND						0xc8
#define HOSTHID_KP_OR							0xc9
#define HOSTHID_KP_LOGICOR						0xca
#define HOSTHID_KP_COLON						0xcb
#define HOSTHID_KP_NUMERAL						0xcc
#define HOSTHID_KP_SPACE						0xcd
#define HOSTHID_KP_AT							0xce
#define HOSTHID_KP_EXCLAMATION					0xcf
#define HOSTHID_KP_MEMSTORE						0xd0
#define HOSTHID_KP_MEMRECALL					0xd1
#define HOSTHID_KP_MEMCLEAR						0xd2
#define HOSTHID_KP_MEMADD						0xd3
#define HOSTHID_KP_MEMSUBTRACT					0xd4
#define HOSTHID_KP_MEMMULTIPLY					0xd5
#define HOSTHID_KP_MEMDIVIDE					0xd6
#define HOSTHID_KP_PLUSMINUS					0xd7
#define HOSTHID_KP_CLEAR						0xd8
#define HOSTHID_KP_CLEARENTRY					0xd9
#define HOSTHID_KP_BINARY						0xda
#define HOSTHID_KP_OCTAL						0xdb
#define HOSTHID_KP_DECIMAL						0xdc
#define HOSTHID_KP_HEXADECIMAL					0xdd
#define HOSTHID_K_LEFTCONTROL					0xe0
#define HOSTHID_K_LEFTSHIFT						0xe1
#define HOSTHID_K_LEFTALT						0xe2
#define HOSTHID_K_LEFTGUI						0xe3
#define HOSTHID_K_RIGHTCONTROL					0xe4
#define	HOSTHID_K_RIGHTSHIFT					0xe5
#define HOSTHID_K_RIGHTALT						0xe6
#define HOSTHID_K_RIGHTGUI						0xe7

#define HOSTHID_U_CTRL							(1 << 0)
#define HOSTHID_U_SHIFT							(1 << 1)
#define HOSTHID_U_ALT							(1 << 2)
#define HOSTHID_U_GUI							(1 << 3)

#define HOSTHID_L_NUMLOCK						0x01
#define HOSTHID_L_CAPSLOCK						0x02
#define HOSTHID_L_SCROLLLOCK					0x03
#define HOSTHID_L_COMPOSE						0x04
#define HOSTHID_L_KANA							0x05
#define HOSTHID_L_SHIFT							0x07

#define HOSTHID_M_X								0x01
#define HOSTHID_M_Y								0x02
#define HOSTHID_M_BUTTON1						0x03
#define HOSTHID_M_BUTTON2						0x04
#define HOSTHID_M_BUTTON3						0x05
#define HOSTHID_M_WHEEL							0x06

#define HOSTHID_J1_X							0x00
#define HOSTHID_J1_Y							0x01
#define HOSTHID_J1_Z							0x02
#define HOSTHID_J1_T							0x03
#define HOSTHID_J1_BUTTON1						0x04
#define HOSTHID_J1_BUTTON2						0x05
#define HOSTHID_J1_BUTTON3						0x06
#define HOSTHID_J1_BUTTON4						0x07
#define HOSTHID_J1_BUTTON5						0x08
#define HOSTHID_J1_BUTTON6						0x09
#define HOSTHID_J1_BUTTON7						0x0a
#define HOSTHID_J1_BUTTON8						0x0b
#define HOSTHID_J1_BUTTON9						0x0c
#define HOSTHID_J1_BUTTON10						0x0d
#define HOSTHID_J1_BUTTON11						0x0e
#define HOSTHID_J1_BUTTON12						0x0f
#define HOSTHID_J2_X							0x10
#define HOSTHID_J2_Y							0x11
#define HOSTHID_J2_Z							0x12
#define HOSTHID_J2_T							0x13
#define HOSTHID_J2_BUTTON1						0x14
#define HOSTHID_J2_BUTTON2						0x15
#define HOSTHID_J2_BUTTON3						0x16
#define HOSTHID_J2_BUTTON4						0x17
#define HOSTHID_J2_BUTTON5						0x18
#define HOSTHID_J2_BUTTON6						0x19
#define HOSTHID_J2_BUTTON7						0x1a
#define HOSTHID_J2_BUTTON8						0x1b
#define HOSTHID_J2_BUTTON9						0x1c
#define HOSTHID_J2_BUTTON10						0x1d
#define HOSTHID_J2_BUTTON11						0x1e
#define HOSTHID_J2_BUTTON12						0x1f
#define HOSTHID_J3_X							0x20
#define HOSTHID_J3_Y							0x21
#define HOSTHID_J3_Z							0x22
#define HOSTHID_J3_T							0x23
#define HOSTHID_J3_BUTTON1						0x24
#define HOSTHID_J3_BUTTON2						0x25
#define HOSTHID_J3_BUTTON3						0x26
#define HOSTHID_J3_BUTTON4						0x27
#define HOSTHID_J3_BUTTON5						0x28
#define HOSTHID_J3_BUTTON6						0x29
#define HOSTHID_J3_BUTTON7						0x2a
#define HOSTHID_J3_BUTTON8						0x2b
#define HOSTHID_J3_BUTTON9						0x2c
#define HOSTHID_J3_BUTTON10						0x2d
#define HOSTHID_J3_BUTTON11						0x2e
#define HOSTHID_J3_BUTTON12						0x2f
#define HOSTHID_J4_X							0x30
#define HOSTHID_J4_Y							0x31
#define HOSTHID_J4_Z							0x32
#define HOSTHID_J4_T							0x33
#define HOSTHID_J4_BUTTON1						0x34
#define HOSTHID_J4_BUTTON2						0x35
#define HOSTHID_J4_BUTTON3						0x36
#define HOSTHID_J4_BUTTON4						0x37
#define HOSTHID_J4_BUTTON5						0x38
#define HOSTHID_J4_BUTTON6						0x39
#define HOSTHID_J4_BUTTON7						0x3a
#define HOSTHID_J4_BUTTON8						0x3b
#define HOSTHID_J4_BUTTON9						0x3c
#define HOSTHID_J4_BUTTON10						0x3d
#define HOSTHID_J4_BUTTON11						0x3e
#define HOSTHID_J4_BUTTON12						0x3f

#define HOSTHID_T_X								0x01
#define HOSTHID_T_Y								0x02
#define HOSTHID_T_BUTTON						0x03

// Structures
typedef struct
{
	int usageId;
	int value;
} HostHIDEvent;

class HostHID : public OEComponent
{
public:
	HostHID();
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	void notify(int notification, OEComponent *component, void *data);
	
	int ioctl(int message, void *data);
	
private:
	OEHostObserver hostObserver;
};
