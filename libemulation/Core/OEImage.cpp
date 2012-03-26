
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

/*OEImage::OEImage(OEData& data)
{
    init();
    
    load(data);
}*/

OEImage::OEImage(OEImage& image, OERect rect)
{
    OERect imageRect = OEMakeRect(0, 0, image.size.width, image.size.height);
    rect = OEIntersectionRect(rect, imageRect);
    
    format = image.format;
    setSize(rect.size);
    
    sampleRate = image.sampleRate;
    interlace = image.interlace;
    blackLevel = image.blackLevel;
    whiteLevel = image.whiteLevel;
    subcarrier = image.subcarrier;
    colorBurst = image.colorBurst;
    phaseAlternation = image.phaseAlternation;
    
    OEUInt32 srcBytesPerRow = image.getBytesPerRow();
    OEUInt32 srcOffset = ((OEUInt32)rect.origin.y * srcBytesPerRow +
                          (OEUInt32)rect.origin.x * image.getBytesPerPixel());
    unsigned char *src = image.getPixels() + srcOffset;
    
    OEUInt32 dstBytesPerRow = getBytesPerRow();
    unsigned char *dst = getPixels();
    
    for (OEUInt32 y = 0; y < size.height; y++)
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
    
    for (OEUInt32 y = 0; y < size.height; y++)
        for (OEUInt32 x = 0; x < size.width; x++)
            setPixel(x, y, image.getPixel(x, y));
}

OEImageFormat OEImage::getFormat()
{
    return format;
}

void OEImage::setSize(OESize value)
{
    size = value;
    
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

OEUInt32 OEImage::getBytesPerPixel()
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

OEUInt32 OEImage::getBytesPerRow()
{
    return getBytesPerPixel() * (OEUInt32)size.width;
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
                        OEUInt8 **rows = (unsigned char **) png_get_rows(png, info);
                        
                        OEUInt8 *dst = getPixels();
                        OEUInt32 dstBytesPerRow = getBytesPerRow();
                        
                        for (int row = 0; row < height; row++)
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

bool load(OEData& data)
{
    return false;
}

void OEImage::print(OEImage& image, OEPoint origin)
{
    OERect rect = OEMakeRect(0, 0, size.width, size.height);
    OERect imageRect = OEMakeRect(origin.x, origin.y, image.size.width, image.size.height);
    
    setSize(OEUnionRect(rect, imageRect).size, 0xff);
    
    for (OEUInt32 y = 0; y < image.size.height; y++)
        for (OEUInt32 x = 0; x < image.size.width; x++)
        {
            OEImagePixel p1 = getPixel(x + origin.x, y + origin.y);
            OEImagePixel p2 = image.getPixel(x, y);
            
            setPixel(x + origin.x, y + origin.y, darken(p1, p2));
        }
    
    return;
}

void OEImage::setSize(OESize value, OEUInt8 fillByte)
{
    OEUInt32 srcBytesPerRow = getBytesPerRow();
    
    OESize oldSize = size;
    size = OEMakeSize((OEUInt32) value.width,
                      (OEUInt32) value.height);
    
    OEUInt32 dstBytesPerRow = getBytesPerRow();
    
    OEUInt32 height = min(oldSize.height, size.height);
    
    if (size.width < oldSize.width)
    {
        OEUInt8 *src = getPixels();
        OEUInt8 *dst = getPixels();
        
        for (OEUInt32 y = 0; y < height; y++)
        {
            memmove(dst, src, dstBytesPerRow);
            
            src += srcBytesPerRow;
            dst += dstBytesPerRow;
        }
        
        pixels.resize(getBytesPerRow() * size.height);
        
        if (size.height > oldSize.height)
            memset(getPixels(), fillByte, getBytesPerRow() * (size.height - oldSize.height));
    }
    else if (size.width > oldSize.width)
    {
        pixels.resize(getBytesPerRow() * size.height);
        
        if (size.height > oldSize.height)
            memset(getPixels() + height * getBytesPerRow(), fillByte,
                   (size.height - oldSize.height) * dstBytesPerRow);
        
        OEUInt8 *src = getPixels() + (height - 1) * srcBytesPerRow;
        OEUInt8 *dst = getPixels() + (height - 1) * dstBytesPerRow;
        
        for (OEInt32 y = (height - 1); y >= 0; y--)
        {
            memmove(dst, src, srcBytesPerRow);
            memset(dst + srcBytesPerRow, fillByte, dstBytesPerRow - srcBytesPerRow);
            
            src -= srcBytesPerRow;
            dst -= dstBytesPerRow;
        }
    }
    else
        pixels.resize(getBytesPerRow() * size.height);
}

OEImagePixel OEImage::getPixel(OEUInt32 x, OEUInt32 y)
{
    OEUInt8 *p = &pixels[y * getBytesPerRow() + x * getBytesPerPixel()];
    
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            return OEImagePixel(p[0], p[0], p[0]);
            
        case OEIMAGE_RGB:
            return OEImagePixel(p[0], p[1], p[2]);
            
        case OEIMAGE_RGBA:
            return OEImagePixel(p[0], p[1], p[2], p[3]);
            
        default:
            return OEImagePixel();
    }
}

void OEImage::setPixel(OEUInt32 x, OEUInt32 y, OEImagePixel value)
{
    OEUInt8 *p = &pixels[y * getBytesPerRow() + x * getBytesPerPixel()];
    
    switch (format)
    {
        case OEIMAGE_LUMINANCE:
            p[0] = ((OEUInt32) value.r +
                    (OEUInt32) value.g +
                    (OEUInt32) value.b) / 3;
            
            break;
            
        case OEIMAGE_RGB:
            p[0] = value.r;
            p[1] = value.g;
            p[2] = value.b;
            
            break;
            
        case OEIMAGE_RGBA:
            p[0] = value.r;
            p[1] = value.g;
            p[2] = value.b;
            p[3] = value.a;
            
            break;
    }
}

OEImagePixel OEImage::darken(OEImagePixel p1, OEImagePixel p2)
{
    OEInt32 r = (OEInt32) p1.r + (OEInt32) p2.r - 255;
    OEInt32 g = (OEInt32) p1.g + (OEInt32) p2.g - 255;
    OEInt32 b = (OEInt32) p1.b + (OEInt32) p2.b - 255;
    
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;
    
    return OEImagePixel(r, g, b);
}

bool OEImage::validatePNGHeader(FILE *fp)
{
    OEUInt8 pngHeader[PNGSIG_BYTENUM];
    
    if (fread(pngHeader, 1, PNGSIG_BYTENUM, fp) != PNGSIG_BYTENUM)
        return false;
    
    return !png_sig_cmp((png_byte *) pngHeader, 0, PNGSIG_BYTENUM);
}
