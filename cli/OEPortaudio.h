
/**
 * OpenEmulator
 * OpenEmulator/portaudio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/portaudio interface.
 */

#include <pthread.h>

#include "OEEmulation.h"

void oepaSetFullDuplex(bool value);

void oepaOpen();
void oepaClose();

OEEmulation *oepaConstruct(string path, string resourcePath);
void oepaDestroy(OEEmulation *emulation);

bool oepaSave(OEEmulation *emulation, string path);
int oepaIoctl(OEEmulation *emulation, string ref, int message, void *data);
bool oepaAddDevices(OEEmulation *emulation,
					string path,
					OEStringRefMap connections);
bool oepaIsDeviceTerminal(OEEmulation *emulation, OERef ref);
bool oepaRemoveDevice(OEEmulation *emulation, OERef ref);
