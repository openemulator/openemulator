
/**
 * libemulation
 * Apple Language Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Language Card
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AppleLanguageCard : public OEComponent
{
public:
	AppleLanguageCard();
	
	bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void dispose();
	
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
    void enableLanguageCard(bool value);
    void updateLanguageCard(bool value);
    
private:
    OEComponent *controlBus;
    OEComponent *memoryBus;
    OEComponent *floatingBus;
    OEComponent *bankSwitcher;
    OEComponent *io;
    
    bool bank1;
    bool ramRead;
    bool preWrite;
    bool ramWrite;
    MemoryMap ramMap;
    
    bool titanIII;
    MemoryMap slot0Map;
    
    void setBank1(bool value);
    void updateBankOffset();
    void updateBankSwitcher();
};
