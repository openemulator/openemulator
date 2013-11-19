
/**
 * libemulation-hal
 * Signal processing vector
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a signal processing vector
 */

#include <math.h>

#include "OEVector.h"

OEVector::OEVector()
{
}

OEVector::OEVector(OEInt size)
{
    data.resize(size);
}

OEVector::OEVector(vector<float> data)
{
    this->data = data;
}

float OEVector::getValue(OEInt i)
{
    return data[i];
}

OEVector OEVector::operator *(const float value)
{
    OEVector w(data);
    
    for (OEInt i = 0; i < data.size(); i++)
        w.data[i] *= value;
    
    return w;
}

OEVector OEVector::operator *(const OEVector& v)
{
    if (v.data.size() != data.size())
        return OEVector();
    
    OEVector w(data);
    
    for (OEInt i = 0; i < data.size(); i++)
        w.data[i] *= v.data[i];
    
    return w;
}

OEVector OEVector::normalize()
{
    float sum = 0;
    
    for (OEInt i = 0; i < data.size(); i++)
        sum += data[i];
    
    OEVector w(data);
    float gain = 1.0F / sum;
    
    for (OEInt i = 0; i < data.size(); i++)
        w.data[i] *= gain;
    
    return w;
}

OEVector OEVector::realIDFT()
{
    OEVector w;
    w.data.resize(data.size());
    
    for (OEInt i = 0; i < data.size(); i++)
    {
        float omega = (float) (2.0F * M_PI * i / data.size());
        
        for (OEInt j = 0; j < data.size(); j++)
            w.data[i] += data[j] * cosf(j * omega);
    }
    
    for (OEInt i = 0; i < data.size(); i++)
        w.data[i] /= (OEInt) data.size();
    
    return w;
}

OEVector OEVector::lanczosWindow(OEInt n, float fc)
{
    if (fc > 0.5F)
        fc = 0.5F;
    
    OEVector v;
    v.data.resize(n);
    
    OESInt halfN = n / 2;
    
    for (OESInt i = 0; i < n; i++)
    {
        float x = 2 * (float) M_PI * fc * (i - halfN);
        
        v.data[i] = (x == 0.0F) ? 1.0F : x = sinf(x) / x;
    }
    
    return v;
}

//
// Based on ideas at:
// http://www.dsprelated.com/showarticle/42.php
//
OEVector OEVector::chebyshevWindow(OEInt n, float sidelobeDb)
{
    OEInt m = n - 1;
    
    OEVector w;
    w.data.resize(m);
    
    float alpha = coshf(acoshf(powf(10, sidelobeDb / 20)) / m);
    
    for (OEInt i = 0; i < m; i++)
    {
        float a = fabsf(alpha * cosf((float) M_PI * i / m));
        if (a > 1)
            w.data[i] = powf(-1, i) * coshf(m * acoshf(a));
        else
            w.data[i] = powf(-1, i) * cosf(m * acosf(a));
    }
    
    w = w.realIDFT();
    
    w.data.resize(n);
    w.data[0] /= 2;
    w.data[n - 1] = w.data[0];
    
    float max = 0;
    
    for (OEInt i = 0; i < n; i++)
        if (fabs(w.data[i]) > max)
            max = fabsf(w.data[i]);
    
    for (OEInt i = 0; i < n; i++)
        w.data[i] /= max;
    
    return w;
}
