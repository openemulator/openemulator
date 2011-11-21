
/**
 * libemulation
 * Joystick interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the joystick interface
 */

#ifndef _MIDIINTERFACE_H
#define _MIDIINTERFACE_H

typedef enum
{
    JOYSTICK_DID_CHANGE,
} JoystickNotification;

#define CANVAS_JOYSTICK_AXIS_NUM	10
#define CANVAS_JOYSTICK_HAT_NUM     1
#define CANVAS_JOYSTICK_BUTTON_NUM	16

typedef struct
{
    OEUInt32 deviceId;
    OEUInt32 usageId;
    float value;
} JoystickHIDNotification;

typedef enum
{
    CANVAS_J_X,
    CANVAS_J_Y,
    CANVAS_J_Z,
    CANVAS_J_RX,
    CANVAS_J_RY,
    CANVAS_J_RZ,
    CANVAS_J_SLIDER,
    CANVAS_J_DIAL,
    CANVAS_J_WHEEL,
    CANVAS_J_HAT,
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
} CanvasJoystickUsageId;

#endif
