
/**
 * libemulation
 * ADC/DAC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an audio ADC/DAC (8 and 16 bit)
 */

#include "OEComponent.h"

class ADCDAC : public OEComponent
{
public:
	ADCDAC();
	
	bool setRef(string name, OEComponent *ref);
	
	void notify(OEComponent *sender, int notification, void *data);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	OEUInt16 read16(OEAddress address);
	void write16(OEAddress address, OEUInt16 value);
	
private:
	OEComponent *audio;
	OEComponent *controlBus;
	
	double phase;
};
