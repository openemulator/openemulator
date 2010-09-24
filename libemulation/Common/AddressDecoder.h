
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
	
	OEUInt32 startAddress;
	OEUInt32 endAddress;
	
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
	
	bool setValue(const string &name, const string &value);
	bool setComponent(const string &name, OEComponent *component);
	
	bool init();
	
	bool postEvent(OEComponent *component, int event, void *data);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	int addressSize;
	int blockSize;
	OEComponent *floatingBus;
	
	AddressDecoderConf conf;
	AddressDecoderRef ref;
	
	int addressMask;
	OEComponents readMap;
	OEComponents writeMap;
	
	void initMap(OEComponent *component);
	void map(AddressDecoderMap *theMap);
	bool map(OEComponent *component, string value);
	
	bool getMap(AddressDecoderMap &theMap, string value);
	bool getMaps(AddressDecoderMaps &theMaps, string value);
};

#endif
