
/**
 * libemulation
 * Audio interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the audio interface
 */

#include <math.h>

float getLevelFromVolume(float value)
{
    if (value != 0)
        return powf(10, (value - 1) * 40 / 20);
    
    return 0;
}
