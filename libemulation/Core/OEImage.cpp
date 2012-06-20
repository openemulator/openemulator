
/**
 * libemulation
 * OEImage
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an image type
 */

#include <png.h>

#include "OEImage.h"

#define PNGSIG_BYTENUM 4

OEImage::OEImage()
{
    init();
}

OEImage::OEImage(string path)
{
    init();
    
    load(path);
}

OEImage::OEImage(OEData& data)
{
    init();
    
    load(data);
}

OEImage::OEImage(OEImage& image, OERect rect)
{
    rect = OEIntegralRect(rect);
    
    OERect oldRect = OEMakeRect(0, 0, image.size.width, image.size.height);
    rect = OEIntersectionRect(rect, oldRect);
    
    format = image.format;
    setSize(rect.size);
    
    sampleRate = image.sampleRate;
    interlace = image.interlace;
    blackLevel = image.blackLevel;
    whiteLevel = image.whiteLevel;
    subcarrier = image.subcarrier;
    colorBurst = image.colorBurst;
    phaseAlternation = image.phaseAlternation;
    
    OEInt srcBytesPerRow = image.getBytesPerRow();
    OEInt srcOffset = (rect.origin.y * srcBytesPerRow +
                       rect.origin.x * image.getBytesPerPixel());
    OEChar *src = image.getPixels() + srcOffset;
    
    OEInt dstBytesPerRow = getBytesPerRow();
    OEChar *dst = getPixels();
    
    for (OEInt y = 0; y < size.height; y++)
    {
        memcpy(dst, src, dstBytesPerRow);
        
        src += srcBytesPerRow;
        dst += dstBytesPerRow;
    }
}

void OEImage::init()
{
    format = OEIMAGE_RGBA;
    size = OEMakeSize(0, 0);
    
    sampleRate = 14318180;
    blackLevel = 0;
    whiteLevel = 1;
    interlace = 0;
    subcarrier = 0;
    
    colorBurst.push_back(0);
    phaseAlternation.push_back(false);
}

void OEImage::setFormat(OEImageFormat value)
{
    OEImage image = *this;
    
    format = value;
    pixels.resize(getBytesPerRow() * size.height);
    
    for (OEInt y = 0; y < size.height; y++)
        for (OEInt x = 0; x < size.width; x++)
            setPixel(x, y, image.getPixel(x, y));
}

OEImageFormat OEImage::getFormat()
{
    return format;
}

void OEImage::setSize(OESize s)
{
    size = OEIntegralSize(s);
    
    pixels.resize(getBytesPerRow() * size.height);
    
    memset(getPixels(), 0, getBytesPerRow() * size.height);
}

OESize OEImage::getSize()
{
    return size;
}

unsigned char *OEImage::getPixels()
{
    return &pixels.front();
}

OEInt OEImage::getBytesPerPixel()
{
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            return 1;
            
        case OEIMAGE_RGB:
            return 3;
            
        case OEIMAGE_RGBA:
            return 4;
            
        default:
            return 0;
    }
}

OEInt OEImage::getBytesPerRow()
{
    return getBytesPerPixel() * (OEInt)size.width;
}

void OEImage::setSampleRate(float value)
{
    sampleRate = value;
}

float OEImage::getSampleRate()
{
    return sampleRate;
}

void OEImage::setBlackLevel(float value)
{
    blackLevel = value;
}

float OEImage::getBlackLevel()
{
    return blackLevel;
}

void OEImage::setWhiteLevel(float value)
{
    whiteLevel = value;
}

float OEImage::getWhiteLevel()
{
    return whiteLevel;
}

void OEImage::setInterlace(float value)
{
    interlace = value;
}

float OEImage::getInterlace()
{
    return interlace;
}

void OEImage::setSubcarrier(float value)
{
    subcarrier = value;
}

float OEImage::getSubcarrier()
{
    return subcarrier;
}

void OEImage::setColorBurst(vector<float> value)
{
    colorBurst = value;
}

vector<float> OEImage::getColorBurst()
{
    return colorBurst;
}

void OEImage::setPhaseAlternation(vector<bool> value)
{
    phaseAlternation = value;
}

vector<bool> OEImage::getPhaseAlternation()
{
    return phaseAlternation;
}

void OEImage::clear()
{
    setSize(OEMakeSize(0, 0));
}

void OEImage::resize(OESize s, OEColor color)
{
    OEInt srcBytesPerRow = getBytesPerRow();
    
    OESize oldSize = size;
    size = OEIntegralSize(s);
    
    OEInt destBytesPerRow = getBytesPerRow();
    
    OEInt height = min(oldSize.height, size.height);
    
    if (size.width < oldSize.width)
    {
        OEChar *src = getPixels();
        OEChar *dest = getPixels();
        
        for (OEInt y = 0; y < height; y++)
        {
            memmove(dest, src, destBytesPerRow);
            
            src += srcBytesPerRow;
            dest += destBytesPerRow;
        }
        
        pixels.resize(getBytesPerRow() * size.height);
    }
    else if (size.width > oldSize.width)
    {
        pixels.resize(getBytesPerRow() * size.height);
        
        OEChar *src = getPixels() + (height - 1) * srcBytesPerRow;
        OEChar *dest = getPixels() + (height - 1) * destBytesPerRow;
        
        for (OESInt y = (height - 1); y >= 0; y--)
        {
            memmove(dest, src, srcBytesPerRow);
            
            for (OEInt x = oldSize.width; x < size.width; x++)
                setPixel(x, y, color);
            
            src -= srcBytesPerRow;
            dest -= destBytesPerRow;
        }
    }
    else
        pixels.resize(getBytesPerRow() * size.height);
    
    for (OEInt y = oldSize.height; y < size.height; y++)
        for (OEInt x = 0; x < size.width; x++)
            setPixel(x, y, color);
}

OEColor OEImage::getPixel(OEInt x, OEInt y)
{
    OEChar *p = &pixels[y * getBytesPerRow() + x * getBytesPerPixel()];
    
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            return OEColor(p[0], p[0], p[0]);
            
        case OEIMAGE_RGB:
            return OEColor(p[0], p[1], p[2]);
            
        case OEIMAGE_RGBA:
            return OEColor(p[0], p[1], p[2], p[3]);
            
        default:
            return OEColor();
    }
}

void OEImage::setPixel(OEInt x, OEInt y, OEColor color)
{
    OEChar *p = &pixels[y * getBytesPerRow() + x * getBytesPerPixel()];
    
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            p[0] = ((OEInt) color.r +
                    (OEInt) color.g +
                    (OEInt) color.b) / 3;
            
            break;
            
        case OEIMAGE_RGB:
            p[0] = color.r;
            p[1] = color.g;
            p[2] = color.b;
            
            break;
            
        case OEIMAGE_RGBA:
            p[0] = color.r;
            p[1] = color.g;
            p[2] = color.b;
            p[3] = color.a;
            
            break;
    }
}

void OEImage::fill(OEColor color)
{
    for (OEInt y = 0; y < size.height; y++)
        for (OEInt x = 0; x < size.width; x++)
            setPixel(x, y, color);
}

void OEImage::blend(OEImage& image, OEPoint origin, OEBlendMode mode)
{
    switch (mode)
    {
        case OEBLEND_NORMAL:
            for (OEInt y = 0; y < image.size.height; y++)
                for (OEInt x = 0; x < image.size.width; x++)
                {
                    OEColor pSrc = image.getPixel(x, y);
                    OEColor pDest = getPixel(x + origin.x, y + origin.y);
                    
                    OEColor p(((OEInt) pSrc.r * pSrc.a +
                               (OEInt) pDest.r * (255 - pSrc.a)) / 255,
                              ((OEInt) pSrc.g * pSrc.a +
                               (OEInt) pDest.g * (255 - pSrc.a)) / 255,
                              ((OEInt) pSrc.b * pSrc.a +
                               (OEInt) pDest.b * (255 - pSrc.a)) / 255,
                              255);
                    
                    setPixel(x + origin.x, y + origin.y, p);
                }
            
            break;
            
        case OEBLEND_DARKEN:
            for (OEInt y = 0; y < image.size.height; y++)
                for (OEInt x = 0; x < image.size.width; x++)
                {
                    OEColor pSrc = image.getPixel(x, y);
                    OEColor pDest = getPixel(x + origin.x, y + origin.y);
                    
                    OEColor p(MIN(pSrc.r, pDest.r),
                              MIN(pSrc.g, pDest.g),
                              MIN(pSrc.b, pDest.b),
                              255);
                    
                    setPixel(x + origin.x, y + origin.y, p);
                }
            
            break;

        case OEBLEND_MULTIPLY:
            for (OEInt y = 0; y < image.size.height; y++)
                for (OEInt x = 0; x < image.size.width; x++)
                {
                    OEColor pSrc = image.getPixel(x, y);
                    OEColor pDest = getPixel(x + origin.x, y + origin.y);
                    
                    OEColor p((OEInt) pSrc.r * pDest.r / 255,
                              (OEInt) pSrc.g * pDest.g / 255,
                              (OEInt) pSrc.b * pDest.b / 255,
                              255);
                    
                    setPixel(x + origin.x, y + origin.y, p);
                }
            
            break;
            
        case OEBLEND_SCREEN:
            for (OEInt y = 0; y < image.size.height; y++)
                for (OEInt x = 0; x < image.size.width; x++)
                {
                    OEColor pSrc = image.getPixel(x, y);
                    OEColor pDest = getPixel(x + origin.x, y + origin.y);
                    
                    OEColor p(255 - (OEInt) (255 - pSrc.r) * (255 - pDest.r) / 255,
                              255 - (OEInt) (255 - pSrc.g) * (255 - pDest.g) / 255,
                              255 - (OEInt) (255 - pSrc.b) * (255 - pDest.b) / 255,
                              255);
                    
                    setPixel(x + origin.x, y + origin.y, p);
                }
            
            break;
    }
    
    return;
}

bool OEImage::load(string path)
{
    bool success = false;
    
    FILE *fp = fopen(path.c_str(), "rb");
    if (fp)
    {
        if (validatePNGHeader(fp))
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
                        png_set_sig_bytes(png, PNGSIG_BYTENUM);
                        png_read_png(png, info, pngTransforms, NULL);
                        png_get_IHDR(png, info,
                                     &width, &height, &bit_depth, &color_type,
                                     NULL, NULL, NULL);
                        
                        if (color_type == PNG_COLOR_TYPE_RGB)
                            format = OEIMAGE_RGB;
                        else
                            format = OEIMAGE_RGBA;
                        size = OEMakeSize(width, height);
                        pixels.resize(getBytesPerRow() * size.height);
                        
                        // Copy image
                        OEChar **rows = (unsigned char **) png_get_rows(png, info);
                        
                        OEChar *dst = getPixels();
                        OEInt dstBytesPerRow = getBytesPerRow();
                        
                        for (OEInt row = 0; row < height; row++)
                        {
                            memcpy(dst, rows[row], dstBytesPerRow);
                            dst += dstBytesPerRow;
                        }
                        
                        success = true;
                    }
                }
            }
            
            png_destroy_read_struct(&png, &info, NULL);
        }
        
        fclose(fp);
    }
    
    return success;
}

bool OEImage::load(OEData& data)
{
    return false;
}

bool OEImage::validatePNGHeader(FILE *fp)
{
    OEChar pngHeader[PNGSIG_BYTENUM];
    
    if (fread(pngHeader, 1, PNGSIG_BYTENUM, fp) != PNGSIG_BYTENUM)
        return false;
    
    return !png_sig_cmp((png_byte *) pngHeader, 0, PNGSIG_BYTENUM);
}
