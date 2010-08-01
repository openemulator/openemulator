
/**
 * libemulation
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "OEComponent.h"

// Events
enum
{
	APPLE1SYSTEM_GET_FRAMEINDEX,
};

// Notifications
enum
{
	APPLE1SYSTEM_VBL,
	APPLE1SYSTEM_RESET,
	APPLE1SYSTEM_NMI,
};

// Receives HOST_AUDIO_BUFFERSTART
// It is possible to configure one CPU
// Calls the CPU
// Translates sample and CPU clocks (clocksPerSample)
// Receives IRQ and NMI requests, and routes them to the CPU
// Receives HOST_EVENTS_RESET_UP and routes it to the CPU

class Apple1System : public OEComponent
{
public:
	Apple1System();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	bool connect(const string &name, OEComponent *component);
	
	void notify(OEComponent *component, int notification, void *data);
	
	int ioctl(int message, void *data);
	
private:
	OEComponent *cpu;
	OEComponent *cpuSocket;
	
	bool isCPUExternal;
	double phase;
};
