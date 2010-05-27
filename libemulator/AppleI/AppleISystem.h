
/**
 * libemulator
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "OEComponent.h"

// Messages
enum
{
	APPLEISYSTEM_GET_SAMPLEINDEX,
};

// Notifications
enum
{
	APPLEISYSTEM_VBL,
};

// Receives HOST_AUDIO_BUFFERSTART
// It is possible to configure one CPU
// Calls the CPU
// Translates sample and CPU clocks (clocksPerSample)
// Receives IRQ and NMI requests, and routes them to the CPU
// Receives HOST_EVENTS_RESET_UP and routes it to the CPU

class AppleISystem : public OEComponent
{
public:
	AppleISystem();
	
	bool setProperty(string name, string value);
	bool getProperty(string name, string &value);
	bool connect(string name, OEComponent *component);
	
	void notify(int notification, OEComponent *component, void *data);
	
	int ioctl(int message, void *data);
	
private:
	OEComponent *cpu;
	OEComponent *cpuSocket;
	
	bool isCPUExternal;
	double phase;
};
