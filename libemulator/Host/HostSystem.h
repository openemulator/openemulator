
/**
 * libemulator
 * Host System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host system events
 */

#include "OEComponent.h"

#define HID_S_POWERDOWN		0x81
#define HID_S_SLEEP			0x82
#define HID_S_WAKEUP		0x83
#define HID_S_CONTEXT		0x84
#define HID_S_MAINMENU		0x85
#define HID_S_APPMENU		0x86
#define HID_S_MENUHELP		0x87
#define HID_S_MENUEXIT		0x88
#define HID_S_MENUSELECT	0x89
#define HID_S_MENURIGHT		0x8a
#define HID_S_MENULEFT		0x8b
#define HID_S_MENUUP		0x8c
#define HID_S_MENUDOWN		0x8d
#define HID_S_COLDRESTART	0x8d
#define HID_S_WARMRESTART	0x8f
#define HID_S_DPADUP		0x90
#define HID_S_DPADDOWN		0x91
#define HID_S_DPADRIGHT		0x92
#define HID_S_DPADLEFT		0x93

#define HID_S_DOCK						0xa0
#define HID_S_UNDOCK					0xa1
#define HID_S_SETUP						0xa2
#define HID_S_BREAK						0xa3
#define HID_S_DEBUGGERBREAK				0xa4
#define HID_S_APPLICATIONBREAK			0xa5
#define HID_S_APPLICATIONDEBUGGERBREAK	0xa6
#define HID_S_SPEAKERMUTE				0xa7
#define HID_S_HIBERNATE					0xa8

#define HID_S_DISPLAYINVERT					0xb0
#define HID_S_DISPLAYINTERNAL				0xb1
#define HID_S_DISPLAYEXTERNAL				0xb2
#define HID_S_DISPLAYBOTH					0xb3
#define HID_S_DISPLAYDUAL					0xb4
#define HID_S_DISPLAYTOGGLEINTEXT			0xb5
#define HID_S_DISPLAYSWAPPRIMARYSECONDARY	0xb6
#define HID_S_DISPLAYLCDAUTOSCALE			0xb7

// Messages
enum
{
	HOST_SYSTEM_UPDATE_DEVICELIST = OEIOCTL_USER,
	HOST_SYSTEM_IS_DEVICELIST_UPDATED,
};
