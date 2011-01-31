
/**
 * libemulation
 * Image
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an image type.
 */

#include <png.h>

#include "OEImage.h"

#define OEIMAGE_PNGSIG_BYTENUM 4

OEImage::OEImage()
{
	format = OEIMAGE_FORMAT_RGBA;
	size = OEMakeSize(0, 0);
}

OEImage::OEImage(const OEImage& image)
{
	format = image.format;
	size = image.size;
	pixels = image.pixels;
}

OEImage& OEImage::operator=(const OEImage& image)
{
	if (this != &image)
	{
		format = image.format;
		size = image.size;
		pixels = image.pixels;
	}
	
	return *this;
}

void OEImage::setFormat(OEImageFormat format)
{
	this->format = format;
	
	update();
}

OEImageFormat OEImage::getFormat()
{
	return format;
}

void OEImage::setSize(OESize size)
{
	this->size = size;
	
	update();
}

OESize OEImage::getSize()
{
	return size;
}

void *OEImage::getPixels()
{
	return &pixels.front();
}

bool OEImage::readFile(string path)
{
	bool result = false;
	
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp)
	{
		if (validatePNG(fp))
		{
			png_structp png = NULL;
			png_infop info = NULL;
			int pngTransforms = (PNG_TRANSFORM_STRIP_16 |
								 PNG_TRANSFORM_PACKING |
								 PNG_TRANSFORM_EXPAND |
								 PNG_TRANSFORM_GRAY_TO_RGB);
			
			png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
										 NULL,
										 NULL,
										 NULL);
			if (png)
			{
				info = png_create_info_struct(png);
				if (info)
				{
					if (setjmp(png_jmpbuf(png)) == 0)
					{
						png_uint_32 width, height;
						int bit_depth, color_type;
						
						png_init_io(png, fp);
						png_set_sig_bytes(png, OEIMAGE_PNGSIG_BYTENUM);
						png_read_png(png, info, pngTransforms, NULL);
						png_get_IHDR(png, info,
									 &width, &height, &bit_depth, &color_type,
									 NULL, NULL, NULL);
						
						format = ((color_type == PNG_COLOR_TYPE_RGB) ? 
								  OEIMAGE_FORMAT_RGB : OEIMAGE_FORMAT_RGBA);
						size.width = width;
						size.height = height;
						
						update();
						
						// Copy image
						int bytesPerPixel = ((color_type == PNG_COLOR_TYPE_RGB) ? 
											 3 : 4);
						int rowByteNum = width * bytesPerPixel;
						char **rows = (char **) png_get_rows(png, info);
						char *pixelsp = (char *) getPixels();
						
						for (int row = 0; row < height; row++)
						{
							memcpy(pixelsp, rows[row], rowByteNum);
							pixelsp += rowByteNum;
						}
						
						result = true;
					}
				}
			}
			
			png_destroy_read_struct(&png, &info, NULL);
		}
		
		fclose(fp);
	}
	
	return result;
}

bool OEImage::validatePNG(FILE *fp)
{
	char pngHeader[OEIMAGE_PNGSIG_BYTENUM];
	
	if (fread(pngHeader, 1, OEIMAGE_PNGSIG_BYTENUM, fp) !=
		OEIMAGE_PNGSIG_BYTENUM)
		return false;
	
	return !png_sig_cmp((png_byte *) pngHeader, 0, OEIMAGE_PNGSIG_BYTENUM);
}

void OEImage::update()
{
	int bytesPerPixel = 0;
	
	if (format == OEIMAGE_FORMAT_LUMINANCE)
		bytesPerPixel = 1;
	else if (format == OEIMAGE_FORMAT_RGB)
		bytesPerPixel = 3;
	else if (format == OEIMAGE_FORMAT_RGBA)
		bytesPerPixel = 4;
	
	pixels.resize(size.width * size.height * bytesPerPixel);
}
