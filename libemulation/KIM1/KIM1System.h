
/**
 * libemulation
 * KIM1System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a KIM-1 System
 */

#include "OEComponent.h"

// Events
enum
{
	KIM1_ASSERT_RESET,
	KIM1_ASSERT_IRQ,
	KIM1_ASSERT_NMI,
	KIM1_GET_FRAMEINDEX,
};

// Notifications
enum
{
	APPLE1SYSTEM_VBL,
	APPLE1SYSTEM_RESET,
	APPLE1SYSTEM_NMI,
};

class KIM1System : public OEComponent
{

};
