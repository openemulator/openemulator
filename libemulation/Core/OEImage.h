
/**
 * libemulation
 * Image
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an image type.
 */

#ifndef _OEIMAGE_H
#define _OEIMAGE_H

#include <string>
#include <vector>

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
	OEIMAGE_FORMAT_LUMINANCE,
	OEIMAGE_FORMAT_NTSC1,
	OEIMAGE_FORMAT_NTSC2,
	OEIMAGE_FORMAT_PAL1,
	OEIMAGE_FORMAT_PAL2,
	OEIMAGE_FORMAT_PAL3,
	OEIMAGE_FORMAT_PAL4,
	OEIMAGE_FORMAT_RGB,
	OEIMAGE_FORMAT_RGBA,
} OEImageFormat;

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

class OEImage
{
public:
	OEImage();
	OEImage(const OEImage& image);
	OEImage& operator=(const OEImage& image);
	
	void setFormat(OEImageFormat format);
	OEImageFormat getFormat();
	
	void setSize(OESize size);
	OESize getSize();
	
	void *getPixels();
	
	bool readFile(string path);
	
private:
	OEImageFormat format;
	OESize size;
	vector<char> pixels;
	
	void update();
	bool validatePNG(FILE *fp);
};

#endif
