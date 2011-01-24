
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
	OEIMAGE_FORMAT_MONOCHROME,
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

class OEImage
{
public:
	OEImage();
	
	void setFormat(OEImageFormat format);
	OEImageFormat getFormat();
	
	void setSize(OESize size);
	OESize getSize();
	
	void *getData();
	
	bool readFile(string path);
	
private:
	OEImageFormat format;
	OESize size;
	vector<char> data;
	
	void update();
	bool validatePNG(FILE *fp);
};

#endif
