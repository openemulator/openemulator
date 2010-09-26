
/**
 * libemulation
 * ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls read only memory
 */

#include "OEComponent.h"

class ROM : public OEComponent
{
public:
	ROM();
	~ROM();
	
	bool setData(string name, OEData *data);
	
	OEUInt8 read(OEAddress address);
	
private:
	OEData *memory;
	
	int mask;
	OEUInt8 *datap;
	
	void setMemory(OEData *data);
};
