
/**
 * libemulation
 * MIDI interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host MIDI interface
 */

class OEMIDI
{
	virtual int send(char *buffer, int bufferSize);
	virtual int receive(char *buffer, int bufferSize);
};
