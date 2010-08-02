
/**
 * libemulation
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "OEComponent.h"

class ROM : public OEComponent
{
public:
	ROM();
	~ROM();
	
	bool setProperty(const string &name, const string &value);
	bool setResource(const string &name, OEData *data);
	bool connect(const string &name, OEComponent *component);
	
	OEUInt8 read(int address);
	
private:
	OEComponent *mmu;
	string mmuMap;
	
	OEData *memory;
	
	int mask;
	char *datap;
	
	void setMemory(OEData *data);
};
