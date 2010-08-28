
/**
 * libemulation
 * Address decoder
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#ifndef _ADDRESSDECODER_H
#define _ADDRESSDECODER_H

#include "OEComponent.h"

// Events
enum {
	ADDRESSDECODER_MAP,
	ADDRESSDECODER_EVENT_END,
};

typedef struct
{	
	bool read;
	bool write;
	
	OEUInt32 start;
	OEUInt32 end;
} OEAddressRange;

typedef vector<OEAddressRange> OEAddressRanges;

class AddressDecoder : public OEComponent
{
public:
	AddressDecoder();
	
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);
	
	bool postEvent(OEComponent *component, int event, void *data);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	OEComponent *bus;
	map<string, string> componentMap;
	
	int mask;
	int shift;
	
	OEComponents readMap;
	OEComponents writeMap;
	
	void mapFloatingBus(OEComponent *bus);
	bool mapComponent(OEComponent *component, const string &value);
	
	bool getAddressRange(OEAddressRange &range, const string &value);
	bool getAddressRanges(OEAddressRanges &ranges, const string &value);
};

#endif
