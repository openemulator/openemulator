
/**
 * libemulation
 * Image loading
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Provides support for image loading.
 */

#ifndef _OEIMAGE_H
#define _OEIMAGE_H

#include <string>

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
} OEImageFormat;

typedef struct
{
	OESize size;
	OEImageFormat format;
	char *data;
} OEImage;

bool oeReadImage(string path, OEImage *image);
void oeFreeImage(OEImage *image);

#endif
