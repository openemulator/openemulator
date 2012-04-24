
/**
 * libemulator
 * Apple II Game Port 
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II Game Port
 */

#include "OEComponent.h"

class AppleIIGamePort : public OEComponent
{
public:
    AppleIIGamePort();
    
	bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	bool postMessage(OEComponent *sender, int message, void *data);
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *controlBus;
	OEComponent *floatingBus;
    OEComponent *gamePort;
	
    float pdl[4];
    bool pb[4];
	bool an[4];
    
    OELong timerStart;
    
    void setAN(OEInt index, bool value);
    void setPDL(OEInt index, float value);
    void setPB(OEInt index, bool value);
    bool isTimerPending(OEInt index);
    void resetTimer();
};
