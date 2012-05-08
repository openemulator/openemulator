
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include "OEComponent.h"

class VidexVideoterm : public OEComponent
{
public:
	VidexVideoterm();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool init();
    
private:
    bool registerSelect;
    OEInt ramBank;
    
    string characterSet1;
    string characterSet2;
};
