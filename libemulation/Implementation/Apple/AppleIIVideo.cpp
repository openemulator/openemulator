
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II video.
 */

#include "AppleIIVideo.h"

bool AppleIIVideo::setValue(string name, string value)
{
	if (name == "tvSystem")
		palTiming = (value == "PAL");
	else if (name == "characterSet")
		characterSet = value;
	else
		return false;
	
	return true;
}

bool AppleIIVideo::getValue(string name, string& value)
{
	if (name == "tvSystem")
		value = palTiming ? "PAL" : "NTSC";
	else if (name == "characterSet")
		value = characterSet;
	else
		return false;
	
	return true;
}

bool AppleIIVideo::setRef(string name, OEComponent *ref)
{
	if (name == "ram00")
		ram = ref;
	else if (name == "ram20")
		ram = ref;
	else if (name == "ram30")
		ram = ref;
	else if (name == "ram40")
		ram = ref;
	else if (name == "ram50")
		ram = ref;
	else if (name == "charset")
		charset = ref;
	else if (name == "charsetSocket")
		charsetSocket = ref;
	else if (name == "monitor")
		monitor = ref;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIVideo::read(OEAddress address)
{
	return 0;
}

void AppleIIVideo::write(OEAddress address, OEUInt8 value)
{
}
