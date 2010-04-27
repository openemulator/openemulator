
/**
 * libemulator
 * Host System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host system events
 */

#include "OEComponent.h"

#define HOSTSYSTEM_POWER		0x00
#define HOSTSYSTEM_PAUSE		0x01
#define HOSTSYSTEM_RESET		0x02
#define HOSTSYSTEM_INTERRUPT	0x03

// Messages
enum
{
	HOSTSYSTEM_UPDATE_DML = OEIOCTL_USER,
	HOSTSYSTEM_QUERY_DML_UPDATE,
	HOSTSYSTEM_ADD_RUNTIME,
	HOSTSYSTEM_GET_RUNTIME,
	HOSTSYSTEM_SET_PAUSE,
	HOSTSYSTEM_GET_PAUSE,
};

// Notifications
enum
{
	HOSTSYSTEM_EVENT,
};

typedef struct
{
	int event;
	bool isDown;
} OEIoctlHostSystemEvent;

class HostSystem : public OEComponent
{
public:
	HostSystem();
	
	int ioctl(int message, void *data);
	
private:
	double runTime;
	bool isPaused;
	
	bool isDMLUpdated;
};
