
/**
 * libemulation
 * Apple I input/output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple I input/output PIA memory
 */

#include "OEComponent.h"

class Apple1IO : public OEComponent
{
public:
    Apple1IO();
    
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
    bool setRef(string name, OEComponent *ref);

    void notify(OEComponent *sender, int notification, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);

private:
    OEComponent *pia;
    OEComponent *terminal;
    
    bool enhancedTerminalSpeed;
    bool fullASCIIKeyboard;
    
    OEUInt8 termKey;
    OEUInt8 termChar;
};
