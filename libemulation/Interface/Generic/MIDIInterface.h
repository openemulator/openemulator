
/**
 * libemulation
 * MIDI interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the MIDI interface
 */

#ifndef _MIDIINTERFACE_H
#define _MIDIINTERFACE_H

typedef enum
{
	MIDI_SEND,
} MIDIMessage;

typedef enum
{
	MIDI_DID_RECEIVE_DATA,
} MIDINotification;

#endif
