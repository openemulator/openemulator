
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <dirent.h>
#include <iostream>

#include "libxml/encoding.h"

#include "emulator.h"

Emulation::Emulation()
{
}

Emulation::~Emulation()
{
}

bool Emulation::readDML(string path, DMLInfo &dmlInfo)
{
	return false;
}

bool Emulation::readTemplate(string path, DMLInfo &dmlInfo)
{
	return false;
}

bool Emulation::open(string emulationPath)
{
	return true;
}

bool Emulation::runFrame()
{
	return true;
}

bool Emulation::save(string emulationPath)
{
	return true;
}

bool Emulation::ioctl(string componentName, int message, void * data)
{
	return 0;
}

bool Emulation::getOutlets(vector<DMLOutlet> &outlets)
{
	return true;
}

bool Emulation::getAvailableDMLs(map<string, DMLInfo> &dmls,
								 vector<string> &availableDMLs)
{
	return true;
}

bool Emulation::getAvailableInlets(DMLOutlet &outlet,
								   vector<DMLInlet> &availableInlets)
{
	return true;
}

bool Emulation::addDevice(string dmlPath,
						  map<string, string> outletInletMap)
{
	return true;
}

void Emulation::removeDevice(string deviceName)
{
}
