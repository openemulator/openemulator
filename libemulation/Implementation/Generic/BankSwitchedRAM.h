
/**
 * libemulation
 * Bank Switched RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls bank-switched random access memory
 */

#include "RAM.h"

#include "MemoryInterface.h"

typedef list<BankSwitchedRAMMap> BankSwitchedRAMMaps;

class BankSwitchedRAM : public RAM
{
public:
    BankSwitchedRAM();
    
    bool setValue(string name, string value);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEAddress blockSize;
    
    OEInt blockBits;
    
    vector<OESLong> offset;
    OESLong *offsetp;
    
    BankSwitchedRAMMaps ramMaps;
    
    bool mapMemory(BankSwitchedRAMMap& value);
};
