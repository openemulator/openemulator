
/**
 * libemulation
 * MM58167
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulates a MM58167 real time clock
 */

#include "MM58167.h"

MM58167::MM58167()
{
}

OEChar MM58167::read(OEAddress address)
{
    switch (address & 0x1f)
    {
        case 0x00:
            break;
    }
    
    return 0;
}

void MM58167::write(OEAddress address, OEChar value)
{
    switch (address & 0x1f)
    {
        case 0x00:
            break;
    }
}
