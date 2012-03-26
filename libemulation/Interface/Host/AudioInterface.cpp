
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
        return pow(10.0, (value - 1.0) * 40.0 / 20.0);
    
    return 0;
}
