
/**
 * libemulation
 * Composite Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a composite monitor.
 */

#include "OEComponent.h"

typedef enum
{
	COMPOSITE_1BIT_VIDEO,
	COMPOSITE_4BIT_VIDEO,
} CompositeFormat;

typedef struct
{
	OEUInt32 *framebuffer;
	int framebufferWidth;
	int framebufferHeight;
	CompositeFormat framebufferFormat;
	
	bool colorCarrier;
} CompositeVideo;

class CompositeMonitor : public OEComponent
{
};
