
/**
 * libemulation
 * Control Bus Interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the control bus interface
 */

// Notes:
// * scheduleTimer schedules a timer using the ControlBusTimer structure
// * invalidateTimers receives the id of the timers to be removed
// * timerDidFire passes the timer using ControlBusTimer
//   (cycles is the number of remaining cycles for this timer)

#ifndef _CONTROLBUSINTERFACE_H
#define _CONTROLBUSINTERFACE_H

typedef struct
{
    OESLong cycles;
    OEInt id;
} ControlBusTimer;

typedef enum
{
    CONTROLBUS_SET_POWERSTATE,
    CONTROLBUS_GET_POWERSTATE,
    
    CONTROLBUS_SET_CLOCKFREQUENCY,
    CONTROLBUS_GET_CLOCKFREQUENCY,
    
    CONTROLBUS_GET_CYCLES,
    CONTROLBUS_GET_AUDIOBUFFERFRAME,
    
    CONTROLBUS_SCHEDULE_TIMER,
    CONTROLBUS_INVALIDATE_TIMERS,
    
    CONTROLBUS_SET_CPUCLOCKMULTIPLIER,
    
    CONTROLBUS_ASSERT_RESET,
    CONTROLBUS_CLEAR_RESET,
    CONTROLBUS_ASSERT_IRQ,
    CONTROLBUS_CLEAR_IRQ,
    CONTROLBUS_ASSERT_NMI,
    CONTROLBUS_CLEAR_NMI,
    CONTROLBUS_IS_RESET_ASSERTED,
    CONTROLBUS_IS_IRQ_ASSERTED,
    CONTROLBUS_IS_NMI_ASSERTED,
    
    CONTROLBUS_END,
} ControlBusMessage;

typedef enum
{
    CONTROLBUS_POWERSTATE_DID_CHANGE,
    CONTROLBUS_CLOCKFREQUENCY_DID_CHANGE,
    CONTROLBUS_TIMER_DID_FIRE,
    CONTROLBUS_RESET_DID_ASSERT,
    CONTROLBUS_RESET_DID_CLEAR,
    CONTROLBUS_IRQ_DID_CHANGE,
    CONTROLBUS_NMI_DID_ASSERT,
    CONTROLBUS_NMI_DID_CLEAR,
} ControlBusNotification;

typedef enum
{
    CONTROLBUS_POWERSTATE_ON,
    CONTROLBUS_POWERSTATE_PAUSE,
    CONTROLBUS_POWERSTATE_STANDBY,
    CONTROLBUS_POWERSTATE_SLEEP,
    CONTROLBUS_POWERSTATE_HIBERNATE,
    CONTROLBUS_POWERSTATE_OFF,
} ControlBusPowerState;

#endif
