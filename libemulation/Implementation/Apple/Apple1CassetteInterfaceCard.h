
/**
 * libemulation
 * Apple-1 Apple Cassette Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include "OEComponent.h"

class Apple1CassetteInterfaceCard : public OEComponent
{
public:
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
};
