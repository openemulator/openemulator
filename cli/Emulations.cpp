/*
 *  Emulations.cpp
 *  OpenEmulator
 *
 *  Created by Marc S. Reßl on 11/02/10.
 *  Copyright 2010 ITBA. All rights reserved.
 *
 */

#ifdef WIN32
#else

#include <pthread.h>
#endif

#include "Emulations.h"
#include "Emulation.h"

vector<OEEmulation *> emulations;
pthread_mutex_t emulationsMutex;

void emulationsOpen()
{
	// Create mutex
	pthread_mutex_init(emulationsMutex, NULL);

}

void emulationsClose()
{
	pthread_mutex_destroy(emulationsMutex);
}


OEEmulation *emulationAdd()
{
}

bool emulationRemove(OEEmulation *emulation)
{
}

bool emulationIoctl()
{
}

bool emulationAddDevices(OEEmulation *emlation, string path)
{
	
}

bool emulationIsDeviceTerminal(OEEmulation *emlation, string path)
{
	
}

bool emulationRemoveDevice(OEEmulation *emlation, string path)
{
	
}
