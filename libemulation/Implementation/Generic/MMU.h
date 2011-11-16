
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

#include <list>

#include "OEComponent.h"

#include "MemoryInterface.h"

typedef list<MemoryMap> MMUMaps;
typedef map<string, string> MMUConf;
typedef map<string, OEComponent *> MMURef;

class MMU : public OEComponent
{
public:
    MMU();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
private:
    MMUConf conf;
    
    OEComponent *addressDecoder;
    MMURef ref;
    
    MMUMaps memoryMaps;
    
    bool getMemoryMap(MemoryMap& theMap, OEComponent *component, string value);
    bool mapRef(OEComponent *component, string conf);
    
    bool addMemoryMap(MemoryMap *value);
    bool removeMemoryMap(MemoryMap *value);
    void unmap(MemoryMap *value);
};

#endif
