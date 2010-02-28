
/**
 * libemulator
 * IOU
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic input-output unit
 */

#define IOU_MAPSIZE		0x100
#define IOU_MAPMASK		0xff

#include "OEComponent.h"

class IOU : public OEComponent
{
public:
	IOU();
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	int offset;
	OEComponent *floatingBus;
	OEComponent *readMap[IOU_MAPSIZE];
	OEComponent *writeMap[IOU_MAPSIZE];
	
	void setFloatingBus(OEComponent *component);
	void setRange(OEComponent *component, int start, int end);
};
