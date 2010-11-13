
/**
 * libemulation
 * Ethernet interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host ethernet interface
 */

class OEEthernet
{
	virtual int send(char *buffer, int bufferSize);
	virtual int receive(char *buffer, int bufferSize);
};
