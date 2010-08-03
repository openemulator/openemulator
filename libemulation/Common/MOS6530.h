
/**
 * libemulation
 * MOS6530
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6530 (ROM, RAM, I/O, Timer)
 */

#include "OEComponent.h"

class MOS6530 : public OEComponent
{
public:
	MOS6530();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	bool connect(const string &name, OEComponent *component);

	int read(int address);
	void write(int address, int value);
	
private:
	OEComponent *mmu;
	string mmuMap;
	
	OEComponent *bus;
	
	OEComponent *portA;
	int directionA;
	int dataA;
	
	OEComponent *portB;
	int directionB;
	int dataB;
	OEComponent *busB;
};
