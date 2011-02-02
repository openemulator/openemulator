
/**
 * libemulation
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component definition
 */

#include "math.h"
#include <sstream>

#include "OEComponent.h"

OEComponent::~OEComponent()
{
}

bool OEComponent::setValue(string name, string value)
{
	return false;
}

bool OEComponent::getValue(string name, string &value)
{
	return false;
}

bool OEComponent::setRef(string name, OEComponent *ref)
{
	return false;
}

bool OEComponent::setData(string name, OEData *data)
{
	delete data;
	
	return false;
}

bool OEComponent::getData(string name, OEData **data)
{
	return false;
}

bool OEComponent::init()
{
	return true;
}

bool OEComponent::postMessage(OEComponent *sender, int message, void *data)
{
	for (OEComponents::iterator i = delegates[message].begin();
		 i != delegates[message].end();
		 i++)
	{
		if ((*i)->postMessage(sender, message, data))
			return true;
	}
	
	return false;
}

bool OEComponent::addDelegate(OEComponent *delegate, int delegation)
{
	delegates[delegation].push_back(delegate);
	
	return true;
}

bool OEComponent::removeDelegate(OEComponent *delegate, int delegation)
{
	OEComponents::iterator first = delegates[delegation].begin();
	OEComponents::iterator last = delegates[delegation].end();
	OEComponents::iterator i = remove(first, last, delegate);
	
	if (i != last)
		delegates[delegation].erase(i, last);
	
	return (i != last);
}

void OEComponent::setDelegate(OEComponent *oldDelegate,
							  OEComponent *newDelegate,
							  int delegation)
{
	if (oldDelegate)
		oldDelegate->removeObserver(this, delegation);
	if (newDelegate)
		newDelegate->addObserver(this, delegation);
}



void OEComponent::notify(OEComponent *sender, int notification, void *data)
{
	for (OEComponents::iterator i = observers[notification].begin();
		 i != observers[notification].end();
		 i++)
		(*i)->notify(sender, notification, data);
}

bool OEComponent::addObserver(OEComponent *observer, int notification)
{
	if (observer)
		observers[notification].push_back(observer);
	
	return true;
}

bool OEComponent::removeObserver(OEComponent *observer, int notification)
{
	OEComponents::iterator first = observers[notification].begin();
	OEComponents::iterator last = observers[notification].end();
	OEComponents::iterator i = remove(first, last, observer);
	
	if (i != last)
		observers[notification].erase(i, last);
	
	return (i != last);
}

void OEComponent::setObserver(OEComponent *oldObserver,
							  OEComponent *newObserver,
							  int notification)
{
	if (oldObserver)
		oldObserver->removeObserver(this, notification);
	if (newObserver)
		newObserver->addObserver(this, notification);
}



OEUInt8 OEComponent::read(OEAddress address)
{
	return 0;
}

void OEComponent::write(OEAddress address, OEUInt8 value)
{
}

OEUInt16 OEComponent::read16(OEAddress address)
{
	return 0;
}

void OEComponent::write16(OEAddress address, OEUInt16 value)
{
}

OEUInt32 OEComponent::read32(OEAddress address)
{
	return 0;
}

void OEComponent::write32(OEAddress address, OEUInt32 value)
{
}

OEUInt64 OEComponent::read64(OEAddress address)
{
	return 0;
}

void OEComponent::write64(OEAddress address, OEUInt64 value)
{
}

int OEComponent::readBlock(OEAddress address, OEData *value)
{
	return 0;
}

int OEComponent::writeBlock(OEAddress address, OEData *value)
{
	return 0;
}



void OEComponent::logMessage(string message)
{
	cerr << "libemulation: " << message << endl;
}

int OEComponent::getInt(const string& value)
{
	if (value.substr(0, 2) == "0x")
	{
		unsigned int i;
		stringstream ss;
		ss << hex << value.substr(2);
		ss >> i;
		return i;
	}
	else
		return atoi(value.c_str());
}

double OEComponent::getFloat(const string& value)
{
	return atof(value.c_str());
}

string OEComponent::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEComponent::getString(float value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEComponent::getHexString(int value)
{
	stringstream ss;
	ss << "0x" << hex << value;
	return ss.str();
}

OEData OEComponent::getCharVector(const string& value)
{
	OEData result;
	int start = (value.substr(0, 2) == "0x") ? 2 : 0;
	int size = value.size() / 2 - start;
	result.resize(size);
	
	for (int i = 0; i < size; i++)
	{
		unsigned int n;
		stringstream ss;
		ss << hex << value.substr(start + i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

int OEComponent::getNextPowerOf2(int value)
{
	return (int) pow(2, ceil(log2(value)));
}
