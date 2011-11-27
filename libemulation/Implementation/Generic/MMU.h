
/**
 * libemulation
 * MMU
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MMU
 */

#ifndef _MMU_H
#define _MMU_H

#include "OEComponent.h"

#include "MemoryInterface.h"

typedef map<string, string> MMUMap;
typedef map<string, OEComponent *> MMURef;

class MMU : public OEComponent
{
public:
    MMU();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
protected:
    bool addMemoryMap(MemoryMap *value);
    bool removeMemoryMap(MemoryMap *value);
    
private:
    MMUMap confMap;
    
    OEComponent *addressDecoder;
    MMURef confRef;
    
    MemoryMaps memoryMaps;
    
    bool getMemoryMap(MemoryMap& theMap, OEComponent *component, string value);
    bool mapConf(OEComponent *component, string value);
    void unmap(MemoryMap *value);
};

#endif
