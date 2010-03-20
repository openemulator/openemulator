/* Configure library by modifying this file */

#ifndef APPLE_NTSC_CONFIG_H
#define APPLE_NTSC_CONFIG_H

/* The following affect the built-in blitter only; a custom blitter can
handle things however it wants. */

/* Bits per pixel of output. Can be 15, 16, 32, or 24 (same as 32). */
#define APPLE_NTSC_OUT_DEPTH 32

/* Type of input pixel values */
#define APPLE_NTSC_IN_T unsigned char

/* Each raw pixel input value is passed through this. You might want to mask
the pixel index if you use the high bits as flags, etc. */
#define APPLE_NTSC_ADJ_IN( in ) in

/* For each pixel, this is the basic operation:
output_color = color_palette [APPLE_NTSC_ADJ_IN( APPLE_NTSC_IN_T )] */

#endif
