
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "emulator.h"

Emulation::Emulation()
{
}

Emulation::~Emulation()
{
}

bool Emulation::readTemplates(string templatesPath,
							  map<string,DMLTemplate> &templates)
{
	DMLTemplate *a = new DMLTemplate;
	
	templates.insert(make_pair("test", a));
	
	return true;
}

bool Emulation::readDMLs(string dmlsPath,
						 map<string,DMLFile> &dmls)
{
	return true;
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

bool Emulation::ioctl(char * componentName, int message, void * data)
{
	return 0;
}

bool Emulation::getOutlets(vector<DMLOutlet> &outlets)
{
	return true;
}

bool Emulation::getAvailableDMLs(map<string, DMLFile> &dmls,
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

void Emulation::removeDevice(char * deviceName)
{
}
