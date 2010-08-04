
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
	
	bool setResource(const string &name, OEData *data);
	
	int read(int address);
	
private:
	OEData *memory;
	
	int mask;
	char *datap;
	
	void setMemory(OEData *data);
};
