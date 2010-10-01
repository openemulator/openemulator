
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
};

typedef struct
{	
	OEComponent *component;
	
	OEAddress startAddress;
	OEAddress endAddress;
	
	bool read;
	bool write;
} AddressDecoderMap;

typedef vector<AddressDecoderMap> AddressDecoderMaps;
typedef map<string, string> AddressDecoderConf;
typedef map<string, OEComponent *> AddressDecoderRef;

class AddressDecoder : public OEComponent
{
public:
	AddressDecoder();
	
	bool setValue(string name, string value);
	bool setComponent(string name, OEComponent *component);
	
	bool init();
	
	bool postEvent(OEComponent *component, int event, void *data);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	int addressSize;
	int blockSize;
	OEComponent *floatingBus;
	
	AddressDecoderConf conf;
	AddressDecoderRef ref;
	
	OEAddress addressMask;
	OEComponents readMap;
	OEComponents writeMap;
	
	void initMap(OEComponent *component);
	void map(AddressDecoderMap *theMap);
	bool map(OEComponent *component, string value);
	
	bool getMaps(AddressDecoderMaps &maps, OEComponent *component, string value);
	bool getMap(AddressDecoderMap &map, OEComponent *component, string value);
};

#endif
