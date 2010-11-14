
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
	HOST_MIDI_RECEIVE,
} HostMIDIMessages;

typedef struct
{
	char *buffer;
	int bufferSize;
	int bufferUsed;
} HostMIDIBuffer;

#endif
