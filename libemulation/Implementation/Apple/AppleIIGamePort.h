
/**
 * libemulator
 * Apple II Game Port 
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II Game Port
 */

#include "OEComponent.h"

#define APPLEIIGAMEPORT_MINVALUE    (0 * 11 + 8)
#define APPLEIIGAMEPORT_MAXVALUE    (255 * 11 + 8)

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
	
protected:
    OEComponent *controlBus;
	OEComponent *floatingBus;
    OEComponent *gamePort;
	
    float pdl[4];
    bool pb[4];
	bool an[4];
    
    OELong timerStart;
    
    void setAN(OELong index, bool value);
    void setPDL(OELong index, float value);
    void setPB(OELong index, bool value);
    bool isTimerPending(OELong index);
    void resetTimer();
};
