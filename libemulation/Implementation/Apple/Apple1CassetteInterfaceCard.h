
/**
 * libemulation
 * Apple I Cassette Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I cassette interface
 */

#include "OEComponent.h"

class Apple1CassetteInterfaceCard : public OEComponent
{
public:
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
};
