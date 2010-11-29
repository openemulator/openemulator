
/**
 * libemulation
 * Host MIDI interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host MIDI interface
 */

#ifndef _HOSTMIDIINTERFACE_H
#define _HOSTMIDIINTERFACE_H

typedef enum
{
	HOST_MIDI_SEND,
} HostMIDIMessages;

typedef enum
{
	HOST_MIDI_DID_RECEIVE_DATA,
} HostMIDINotifications;

#endif
