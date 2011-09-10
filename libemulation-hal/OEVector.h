
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

#include <vector>

using namespace std;

class OEVector
{
public:
    OEVector();
    OEVector(int size);
    OEVector(vector<float> data);
    
    float getValue(unsigned int i);
    
    OEVector operator *(const float value);
    OEVector operator *(const OEVector& v);
    OEVector normalize();
    OEVector realIDFT();
    
    static OEVector lanczosWindow(unsigned int n, float fc);
    static OEVector chebyshevWindow(unsigned int n, float sidelobeDb);
    
private:
    vector<float> data;
};

#endif
