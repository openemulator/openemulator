
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
	
	bool postEvent(OEComponent *component, int message, void *data);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	OEComponent *mmu;
	string mmuMap;
	
	OEComponent *bus;
	
	int mask;
	int shift;
	
	OEComponents readMap;
	OEComponents writeMap;
	
	void setBus(OEComponent *bus);
	
	bool getRange(OEAddressRange &range, const string &value);
	bool getRanges(OEAddressRanges &ranges, const string &value);
};
