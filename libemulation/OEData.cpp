
/**
 * libemulation
 * OEData
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Data class with reference counting
 */

#include "OEData.h"

OEData::OEData()
{
	retainCount = 1;
}

OEData::OEData(int size)
{
	retainCount = 1;
	data.resize(size);
}

OEData::OEData(const OEData &t)
{
	retainCount = 1;
	data = t.data;
}

OEData &OEData::operator=(const OEData &t)
{
	if (this != &t)
	{
		retainCount = 1;
		data = t.data;
	}
	
	return *this;
}

void OEData::retain()
{
	retainCount++;
}

void OEData::release()
{
	retainCount--;
	if (retainCount <= 0)
		delete this;
}

char *OEData::getData()
{
	return &data[0];
}

size_t OEData::size()
{
	return data.size();
}

char &OEData::operator[](const size_t pos)
{
	return data[pos];
}

bool OEData::resize(int size)
{
	data.resize(size);
	return (size == data.size());
}
