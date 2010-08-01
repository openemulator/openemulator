
/**
 * libemulation
 * Memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory map
 */

#include "OEComponent.h"

enum {
	MEMORYMAP_MAP,
};

typedef struct
{	
	bool read;
	bool write;
	
	OEUInt32 start;
	OEUInt32 end;
} OEMemoryRange;

typedef vector<OEMemoryRange> OEMemoryRanges;

class MemoryMap : public OEComponent
{
public:
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);
	
	bool postEvent(OEComponent *component, int message, void *data);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	string mmuMap;
	
	int mask;
	int shift;
	
	vector<OEComponent *> readMap;
	vector<OEComponent *> writeMap;

	bool getRange(OEMemoryRange &range, const string &value);
	bool getRanges(OEMemoryRanges &ranges, const string &value);
};
