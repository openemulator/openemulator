
/**
 * AppleIIGo
 * SDL main
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <stdint.h>

uint32_t isProfilePath(char * path);
void sendSdlMessage(uint32_t sdlIoMessage);
void sendComponentMessage(char * destination, void * messageData);
