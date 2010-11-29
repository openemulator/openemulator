
/**
 * libemulation
 * Character set loader
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Loads and parses a character set.
 */

#include "OEComponent.h"

class CharsetLoader : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setData(string name, OEData *data);
};
