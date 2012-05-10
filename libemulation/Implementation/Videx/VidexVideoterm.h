
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include "MC6845.h"

class VidexVideoterm : public MC6845
{
public:
	VidexVideoterm();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *video;
    OEComponent *ram;
    
    OEInt ramBank;
    
    string characterSet1;
    string characterSet2;
    
    bool loadFont(string name, OEData *data);
    
    void setRAMBank(OEInt value);
    void updateRAMBank();
    
    void updateTimer();
    void updateScreen();
};
