
/**
 * libemulation
 * Image loading
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Provides support for image loading.
 */

#include <png.h>

#include "OEImage.h"

#define OEIMAGE_PNGSIG_BYTENUM 4

bool oeValidatePNG(FILE *fp)
{
	char pngHeader[OEIMAGE_PNGSIG_BYTENUM];
	if (fread(pngHeader, 1, OEIMAGE_PNGSIG_BYTENUM, fp) !=
		OEIMAGE_PNGSIG_BYTENUM)
		return false;
	
	return !png_sig_cmp((png_byte *) pngHeader, 0, OEIMAGE_PNGSIG_BYTENUM);
}

bool oeReadImage(string path, OEImage *image)
{
	bool result = false;
	
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp)
	{
		if (oeValidatePNG(fp))
		{
			png_structp png = NULL;
			png_infop info = NULL;
			int pngTransforms = (PNG_TRANSFORM_STRIP_16 |
								 PNG_TRANSFORM_PACKING |
								 PNG_TRANSFORM_EXPAND);
			
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
						png_init_io(png, fp);
						png_set_sig_bytes(png, OEIMAGE_PNGSIG_BYTENUM);
						png_read_png(png, info, pngTransforms, NULL);
						
						image->size.width = info->width;
						image->size.height = info->height;
						image->format = OEIMAGE_FORMAT_RGB;
						image->data = (char *) malloc(info->width * info->height * 4);
						
						// Copy image
						int rowByteNum = info->width * 4;
						char **rows = (char **) png_get_rows(png, info);
						char *data = image->data;
						
						for (int row = 0; row < info->height; row++)
						{
							memcpy(data, rows[row], rowByteNum);
							data += rowByteNum;
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

void oeFreeImage(OEImage *image)
{
	free(image->data);
	image->data = NULL;
}
