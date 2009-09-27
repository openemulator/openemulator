
/**
 * libemulator
 * Returns a DML Info
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "OEInfo.h"

#define DMLINFO_FILENAME "info.xml"

OEInfo::OEInfo(string path)
{
	open = false;
}

OEInfo::OEInfo(xmlDocPtr dml)
{
}

OEInfo::~OEInfo()
{
}

bool OEInfo::isOpen()
{
	return open;
}

OEProperties OEInfo::getProperties()
{
	OEProperties properties;
	return properties;
}

OEPortProperties OEInfo::getInlets()
{
	OEPortProperties portProperties;
	return portProperties;
}

OEPortProperties OEInfo::getOutlets()
{
	OEPortProperties portProperties;
	return portProperties;
}
