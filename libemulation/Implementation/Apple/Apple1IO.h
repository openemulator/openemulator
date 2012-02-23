
/**
 * libemulation
 * Apple-1 input/output
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Interfaces an Apple-1 terminal with a PIA
 */

#include "OEComponent.h"

class Apple1IO : public OEComponent
{
public:
    Apple1IO();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);

private:
    bool enhancedTerminalSpeed;
    bool fullASCIIKeyboard;
    
    OEComponent *pia;
    OEComponent *terminal;
    
    OEUInt8 terminalKey;
    OEUInt8 terminalChar;
    
    void sendChar();
};
