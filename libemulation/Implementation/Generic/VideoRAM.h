
/**
 * libemulation
 * Video RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls video random access memory
 */

#include "RAM.h"

class VideoRAM : public RAM
{
public:
    VideoRAM();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *observer;
};
