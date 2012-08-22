
/**
 * libemulation
 * MM58167
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MM58167 real time clock
 */

#ifndef MM58167_H
#define MM58167_H

#include "OEComponent.h"

typedef enum
{
    MM58167_IRQ_DID_CHANGE,
} MM58167Notification;

class MM58167 : public OEComponent
{
public:
    MM58167();
};

#endif
