
/**
 * libemulation-hal
 * Signal processing vector
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a signal processing vector
 */

#ifndef _OEVECTOR_H
#define _OEVECTOR_H

#include "OECommon.h"

class OEVector
{
public:
    OEVector();
    OEVector(OEInt size);
    OEVector(vector<float> data);
    
    float getValue(OEInt i);
    
    OEVector operator *(const float value);
    OEVector operator *(const OEVector& v);
    OEVector normalize();
    OEVector realIDFT();
    
    static OEVector lanczosWindow(OEInt n, float fc);
    static OEVector chebyshevWindow(OEInt n, float sidelobeDb);
    
private:
    vector<float> data;
};

#endif
