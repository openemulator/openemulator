
/**
 * libemulation
 * OEImage
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator image type
 */

#ifndef _OEIMAGE_H
#define _OEIMAGE_H

#include "OECommon.h"

using namespace std;

typedef struct
{
    float x;
    float y;
} OEPoint;

typedef struct
{
    float width;
    float height;
} OESize;

typedef struct
{
    OEPoint origin;
    OESize size;
} OERect;

typedef enum
{
    OEIMAGE_LUMINANCE,
    OEIMAGE_RGB,
    OEIMAGE_RGBA,
} OEImageFormat;

typedef struct
{
    float black;
    float white;
} OEImageVideoLevels;

class OEImagePixel
{
public:
    OEImagePixel()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0xff;
    }
    
    OEImagePixel(OEUInt8 l)
    {
        r = l;
        g = l;
        b = l;
        a = 0xff;
    }
    
    OEImagePixel(OEUInt8 r, OEUInt8 g, OEUInt8 b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        a = 0xff;
    }
    
    OEImagePixel(OEUInt8 r, OEUInt8 g, OEUInt8 b, OEUInt8 a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    
    OEUInt8 r;
    OEUInt8 g;
    OEUInt8 b;
    OEUInt8 a;
};

// Macros

inline OEPoint OEMakePoint(float x, float y)
{
    OEPoint p;
    
    p.x = x;
    p.y = y;
    
    return p;
}

inline OESize OEMakeSize(float w, float h)
{
    OESize s;
    
    s.width = w;
    s.height = h;
    
    return s;
}

inline OERect OEMakeRect(float x, float y, float w, float h)
{
    OERect r;
    
    r.origin.x = x;
    r.origin.y = y;
    r.size.width = w;
    r.size.height = h;
    
    return r;
}

inline float OEWidth(OERect aRect)
{
    return aRect.size.width;
}

inline float OEHeight(OERect aRect)
{
    return aRect.size.height;
}

inline float OEMaxX(OERect aRect)
{
    return (aRect.origin.x + aRect.size.width);
}

inline float OEMaxY(OERect aRect)
{
    return (aRect.origin.y + aRect.size.height);
}

inline float OEMinX(OERect aRect)
{
    return (aRect.origin.x);
}

inline float OEMinY(OERect aRect)
{
    return (aRect.origin.y);
}

inline bool OEIsEmptyRect(OERect aRect)
{
    return ((OEWidth(aRect) > 0) && (OEHeight(aRect) > 0)) ? false : true;
}

inline OERect OEIntersectionRect(OERect aRect, OERect bRect)
{
    if (OEMaxX(aRect) <= OEMinX(bRect) ||
        OEMaxX(bRect) <= OEMinX(aRect) ||
        OEMaxY(aRect) <= OEMinY(bRect) ||
        OEMaxY(bRect) <= OEMinY(aRect)) 
        return OEMakeRect(0, 0, 0, 0);
    
    OERect r;
    
    if (OEMinX(aRect) <= OEMinX(bRect))
        r.origin.x = bRect.origin.x;
    else
        r.origin.x = aRect.origin.x;
    
    if (OEMinY(aRect) <= OEMinY(bRect))
        r.origin.y = bRect.origin.y;
    else
        r.origin.y = aRect.origin.y;
    
    if (OEMaxX(aRect) >= OEMaxX(bRect))
        r.size.width = OEMaxX(bRect) - r.origin.x;
    else
        r.size.width = OEMaxX(aRect) - r.origin.x;
    
    if (OEMaxY(aRect) >= OEMaxY(bRect))
        r.size.height = OEMaxY(bRect) - r.origin.y;
    else
        r.size.height = OEMaxY(aRect) - r.origin.y;
    
    return r;
}

#ifndef MAX
#define MAX(a,b) \
({__typeof__(a) _MAX_a = (a); __typeof__(b) _MAX_b = (b);  \
_MAX_a > _MAX_b ? _MAX_a : _MAX_b; })
#define	GS_DEFINED_MAX
#endif

#ifndef MIN
#define MIN(a,b) \
({__typeof__(a) _MIN_a = (a); __typeof__(b) _MIN_b = (b);  \
_MIN_a < _MIN_b ? _MIN_a : _MIN_b; })
#define	GS_DEFINED_MIN
#endif

inline OERect OEUnionRect(OERect aRect, OERect bRect)
{
    OERect r;
    
    if (OEIsEmptyRect(aRect) && OEIsEmptyRect(bRect))
        return OEMakeRect(0, 0, 0, 0);
    else if (OEIsEmptyRect(aRect))
        return bRect;
    else if (OEIsEmptyRect(bRect))
        return aRect;
    
    r = OEMakeRect(MIN(OEMinX(aRect), OEMinX(bRect)),
                   MIN(OEMinY(aRect), OEMinY(bRect)), 0, 0);
    
    r = OEMakeRect(OEMinX(r), OEMinY(r),
                   MAX(OEMaxX(aRect), OEMaxX(bRect)) - OEMinX(r),
                   MAX(OEMaxY(aRect), OEMaxY(bRect)) - OEMinY(r));
    
    return r;
    
}

class OEImage
{
public:
    OEImage();
    OEImage(OEImage& image, OERect rect);
    
    void setFormat(OEImageFormat value);
    OEImageFormat getFormat();
    void setSize(OESize value);
    OESize getSize();
    OEUInt32 getBytesPerPixel();
    OEUInt32 getBytesPerRow();
    OEUInt8 *getPixels();
    
    void setSampleRate(float value);
    float getSampleRate();
    void setVideoLevels(OEImageVideoLevels value);
    OEImageVideoLevels getVideoLevels();
    void setInterlace(float value);
    float getInterlace();
    void setSubcarrier(float value);
    float getSubcarrier();
    void setColorBurst(vector<float> value);
    vector<float> getColorBurst();
    void setPhaseAlternation(vector<bool> value);
    vector<bool> getPhaseAlternation();
    
    bool load(string path);
    void print(OEImage& image, OEPoint origin);
    
private:
    OEImageFormat format;
    OESize size;
    OEData pixels;
    
    float sampleRate;
    OEImageVideoLevels videoLevels;
    float interlace;
    float subcarrier;
    vector<float> colorBurst;
    vector<bool> phaseAlternation;
    
    void setSize(OESize value, OEUInt8 fillByte);
    OEImagePixel getPixel(OEUInt32 x, OEUInt32 y);
    void setPixel(OEUInt32 x, OEUInt32 y, OEImagePixel value);
    OEImagePixel subtractPixel(OEImagePixel p1, OEImagePixel p2);
    bool validatePNGHeader(FILE *fp);
};

#endif
