
/**
 * libemulation
 * Memory interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the address interfaces
 */

#include "MemoryInterface.h"

bool appendMemoryMaps(MemoryMaps& theMaps,
                      OEComponent *component,
                      string value)
{
    MemoryMap m;
    
    m.component = component;
    
    vector<string> items = strsplit(value, ',');
    
    for (vector<string>::iterator i = items.begin();
         i != items.end();
         i++)
    {
        string entry = strtolower(*i);
        
        m.read = (entry.find_first_of('r') != string::npos);
        m.write = (entry.find_first_of('w') != string::npos);
        
        if (!m.read && !m.write)
            m.read = m.write = true;
        
        entry = strfilter(entry, "0123456789abcdef-x");
        
        vector<string> address = strsplit(entry, '-');
        
        if (address.size() == 1)
        {
            if (address[0] == "")
                return false;
            
            m.startAddress = m.endAddress = getOELong(address[0]);
        }
        else if (address.size() == 2)
        {
            if ((address[0] == "") || (address[1] == ""))
                return false;
            
            m.startAddress = getOELong(address[0]);
            m.endAddress = getOELong(address[1]);
        }
        else
            return false;
        
        theMaps.push_back(m);
    }
    
	return true;
}

bool validateMemoryMaps(MemoryMaps& theMaps,
                        OEAddress blockSize,
                        OEAddress addressMask)
{
    OEAddress blockMask = (1 << blockSize) - 1;
    
    for (MemoryMaps::iterator i = theMaps.begin();
         i != theMaps.end();
         i++)
    {
        if ((i->startAddress > i->endAddress) ||
            (i->endAddress > addressMask) ||
            (i->startAddress & blockMask) ||
            ((i->endAddress & blockMask) != blockMask))
            return false;
    }
    
    return true;
}
