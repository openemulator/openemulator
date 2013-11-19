
/**
 * libemulation
 * Apple III Game Port
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple III game port
 */

#ifndef _APPLEIIIGAMEPORT_H
#define _APPLEIIIGAMEPORT_H

#include "AppleIIGamePort.h"

class AppleIIIGamePort : public AppleIIGamePort
{
public:
    AppleIIIGamePort();
    
    OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEInt channelSelect;
    
    void setChannelSelect(OEInt index, bool value);
    bool isTimerPending();
};

#endif
