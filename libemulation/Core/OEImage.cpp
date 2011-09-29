
/**
 * libemulation
 * OEImage
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator image type
 */

#include <png.h>

#include "OEImage.h"

#define OEIMAGE_PNGSIG_BYTENUM 4

OEImage::OEImage()
{
    format = OEIMAGE_RGBA;
    options = 0;
    size = OEMakeSize(0, 0);
}

void OEImage::setFormat(OEImageFormat format)
{
    this->format = format;
    
    updateSize();
}

OEImageFormat OEImage::getFormat()
{
    return format;
}

void OEImage::setOptions(OEImageOptions options)
{
    this->options = options;
}

OEImageOptions OEImage::getOptions()
{
    return options;
}

void OEImage::setSize(OESize size)
{
    this->size.width = (int)size.width;
    this->size.height = (int)size.height;
    
    updateSize();
}

OESize OEImage::getSize()
{
    return size;
}

unsigned char *OEImage::getPixels()
{
    return &pixels.front();
}

int OEImage::getBytesPerPixel()
{
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            return 1;
        case OEIMAGE_RGB:
            return 3;
        case OEIMAGE_RGBA:
            return 4;
    }
    
    return 0;
}

int OEImage::getBytesPerRow()
{
    return getBytesPerPixel() * (int)size.width;
}

bool OEImage::load(string path)
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
                                  OEIMAGE_RGB : OEIMAGE_RGBA);
                        size.width = width;
                        size.height = height;
                        
                        updateSize();
                        
                        // Copy image
                        int bytesPerPixel = (format == OEIMAGE_RGB) ? 3 : 4;
                        int bytesPerRow = width * bytesPerPixel;
                        unsigned char **rows = (unsigned char **) png_get_rows(png, info);
                        unsigned char *pixelsp = (unsigned char *) getPixels();
                        
                        for (int row = 0; row < height; row++)
                        {
                            memcpy(pixelsp, rows[row], bytesPerRow);
                            pixelsp += bytesPerRow;
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

void OEImage::overlay(OEPoint origin, OEImage& image)
{
    if (image.getFormat() != getFormat())
        return;
    
    OESize imageSize = image.getSize();
    OERect rect = OEMakeRect(origin.x,
                             origin.y,
                             imageSize.width,
                             imageSize.height);
    rect = OEIntersectionRect(rect, OEMakeRect(0, 0, size.width, size.height));
    
    bool isAlpha = (image.getFormat() == OEIMAGE_RGBA);
    
    int srcBytesPerRow = image.getBytesPerRow();
    int srcOffset = ((int)(rect.origin.y - origin.y) * srcBytesPerRow +
                     (int)(rect.origin.x - origin.y) * image.getBytesPerPixel());
    unsigned char *src = (unsigned char *)image.getPixels() + srcOffset;
    int dstBytesPerRow = getBytesPerRow();
    int dstOffset = (int)rect.origin.y * srcBytesPerRow +
    (int)rect.origin.x * getBytesPerPixel();
    unsigned char *dst = (unsigned char *)getPixels() + dstOffset;
    int n = (int)OEWidth(rect) * getBytesPerPixel();
    
    for (int y = 0; y < OEHeight(rect); y++)
    {
        for (int i = 0; i < n; i++)
        {
            if (isAlpha && !(~i & 0x3))
                continue;
            int temp = src[i] + dst[i] - 255;
            if (temp < 0)
                temp = 0;
            dst[i] = temp;
        }
        src += srcBytesPerRow;
        dst += dstBytesPerRow;
    }
}

OEImage OEImage::getClip(OERect rect)
{
    OEImage image;
    
    rect = OEIntersectionRect(rect, OEMakeRect(0, 0, size.width, size.height));
    
    image.setFormat(format);
    image.setSize(rect.size);
    
    int srcBytesPerRow = getBytesPerRow();
    int srcOffset = ((int)rect.origin.y * srcBytesPerRow +
                     (int)rect.origin.x * getBytesPerPixel());
    unsigned char *src = getPixels() + srcOffset;
    int dstBytesPerRow = image.getBytesPerRow();
    unsigned char *dst = image.getPixels();
    
    for (int y = 0; y < (int)OEHeight(rect); y++)
    {
        memcpy(dst, src, dstBytesPerRow);
        src += srcBytesPerRow;
        dst += dstBytesPerRow;
    }
    
    return image;
}

void OEImage::updateSize()
{
    int prevSize = (int) pixels.size();
    pixels.resize(getBytesPerRow() * (int)size.height);
    
    if (options & OEIMAGE_PAPER)
    {
        int diff = (int) pixels.size() - prevSize;
        if (diff > 0)
            memset(&pixels.front() + prevSize, 0xff, diff);
    }
}

bool OEImage::validatePNG(FILE *fp)
{
    unsigned char pngHeader[OEIMAGE_PNGSIG_BYTENUM];
    
    if (fread(pngHeader, 1, OEIMAGE_PNGSIG_BYTENUM, fp) !=
        OEIMAGE_PNGSIG_BYTENUM)
        return false;
    
    return !png_sig_cmp((png_byte *) pngHeader, 0, OEIMAGE_PNGSIG_BYTENUM);
}
