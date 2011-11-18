
/**
 * libemulator
 * Apple II Game Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II's game port
 */

#include "OEComponent.h"

class AppleIIGamePort : public OEComponent
{
public:
    AppleIIGamePort();
    
	bool setValue(string name, string value);
    bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	bool postMessage(OEComponent *sender, int message, void *data);
    
    OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
    OEComponent *controlBus;
	OEComponent *floatingBus;
	OEComponent *gamePort;
	
    float pdl[4];
    bool pb[4];
	bool an[4];
    
    OEUInt64 timerStart;
    
    void setAN(int index, bool value);
    bool isTimerExpired(int index);
    void resetTimer();
};
