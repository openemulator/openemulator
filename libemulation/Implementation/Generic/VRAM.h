
/**
 * libemulation
 * Video RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls video random access memory
 */

#include "RAM.h"

class VRAM : public RAM
{
public:
    VRAM();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void write(OEAddress address, OEChar value);
    
private:
    OEAddress videoBlockSize;
    string videoMap;
    OEComponent *videoObserver;
    
    OEInt videoBlockBits;
    
    vector<OEChar> notifyMap;
    
    OEChar *notifyMapp;
};
