
/**
 * libemulation
 * Address decoder
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#include "OEComponent.h"

enum {
	ADDRESSDECODER_MAP,
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
	
	void mapBus(OEComponent *bus);
	bool mapComponent(OEComponent *component, const string &value);
	
	bool getAddressRange(OEAddressRange &range, const string &value);
	bool getAddressRanges(OEAddressRanges &ranges, const string &value);
};
