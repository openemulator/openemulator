/* Apple II NTSC video filter */

/* apple_ntsc 0.1.0 */
#ifndef APPLE_NTSC_H
#define APPLE_NTSC_H

#include "apple_ntsc_config.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Image parameters, ranging from -1.0 to 1.0. Actual internal values shown
in parenthesis and should remain fairly stable in future versions. */
typedef struct apple_ntsc_setup_t
{
	/* Basic parameters */
	double hue;        /* -1 = -180 degrees     +1 = +180 degrees */
	double saturation; /* -1 = grayscale (0.0)  +1 = oversaturated colors (2.0) */
	double contrast;   /* -1 = dark (0.5)       +1 = light (1.5) */
	double brightness; /* -1 = dark (0.5)       +1 = light (1.5) */
	double sharpness;  /* edge contrast enhancement/blurring */
	
	/* Advanced parameters */
	double gamma;      /* NOT CURRENTLY SUPPORTED */
	double resolution; /* image resolution */
	double artifacts;  /* artifacts caused by color changes */
	double fringing;   /* color artifacts caused by brightness changes */
	double bleed;      /* color bleed (color resolution reduction) */
	float const* decoder_matrix; /* optional RGB decoder matrix, 6 elements */
	
	unsigned char* palette_out;  /* optional RGB palette out, 3 bytes per color */
} apple_ntsc_setup_t;

/* Video format presets */
extern apple_ntsc_setup_t const apple_ntsc_composite; /* color bleeding + artifacts */
extern apple_ntsc_setup_t const apple_ntsc_svideo;    /* color bleeding only */
extern apple_ntsc_setup_t const apple_ntsc_rgb;       /* crisp image */
extern apple_ntsc_setup_t const apple_ntsc_monochrome;/* desaturated + artifacts */

enum { apple_ntsc_palette_size = 16 };

/* Initialize and adjust parameters. Can be called multiple times on the same
apple_ntsc_t object. Can pass NULL for either parameter. */
typedef struct apple_ntsc_t apple_ntsc_t;
void apple_ntsc_init( apple_ntsc_t* ntsc, apple_ntsc_setup_t const* setup );

/* Filters one or more rows of pixels. Input pixels are 4-bit palette colors.
In_row_width is the number of pixels to get to the next input row. Out_pitch
is the number of *bytes* to get to the next output row. Output pixel format
is set by APPLE_NTSC_OUT_DEPTH (defaults to 16-bit RGB). */
void apple_ntsc_blit( apple_ntsc_t const* ntsc, APPLE_NTSC_IN_T const* input,
		long in_row_width, int in_width, int height, void* rgb_out, long out_pitch );

/* Number of output pixels written by blitter for given input width. Width might
be rounded down slightly; use APPLE_NTSC_IN_WIDTH() on result to find rounded
value. Guaranteed not to round 140 down at all. */
#define APPLE_NTSC_OUT_WIDTH( in_width ) \
	(((in_width) / apple_ntsc_in_chunk + 1) * apple_ntsc_out_chunk)

/* Number of input pixels that will fit within given output width. Might be
rounded down slightly; use APPLE_NTSC_OUT_WIDTH() on result to find rounded
value. */
#define APPLE_NTSC_IN_WIDTH( out_width ) \
	(((out_width) / apple_ntsc_out_chunk - 1) * apple_ntsc_in_chunk)


/* Interface for user-defined custom blitters */

enum { apple_ntsc_in_chunk  = 2 }; /* number of input pixels read per chunk */
enum { apple_ntsc_out_chunk = 7 }; /* number of output pixels generated per chunk */
enum { apple_ntsc_black     = 0 }; /* palette index for black */

/* Begins outputting row and start three pixels. First pixel will be cut off a bit.
Use apple_ntsc_black for unused pixels. Declares variables, so must be before first
statement in a block (unless you're using C++). */
#define APPLE_NTSC_BEGIN_ROW( ntsc, pixel0, pixel1 ) \
	unsigned const md_pixel0_ = (pixel0);\
	apple_ntsc_rgb_t const* kernel0  = APPLE_NTSC_ENTRY_( ntsc, md_pixel0_ );\
	unsigned const md_pixel1_ = (pixel1);\
	apple_ntsc_rgb_t const* kernel1  = APPLE_NTSC_ENTRY_( ntsc, md_pixel1_ );\
	apple_ntsc_rgb_t const* kernelx0;\
	apple_ntsc_rgb_t const* kernelx1 = kernel0

/* Begins input pixel */
#define APPLE_NTSC_COLOR_IN( in_index, ntsc, color_in ) \
	APPLE_NTSC_COLOR_IN_( in_index, color_in, APPLE_NTSC_ENTRY_, ntsc )

/* Generates output pixel. Bits can be 24, 16, 15, 32 (treated as 24), or 0:
24:          RRRRRRRR GGGGGGGG BBBBBBBB (8-8-8 RGB)
16:                   RRRRRGGG GGGBBBBB (5-6-5 RGB)
15:                    RRRRRGG GGGBBBBB (5-5-5 RGB)
 0: xxxRRRRR RRRxxGGG GGGGGxxB BBBBBBBx (native internal format; x = junk bits) */
#define APPLE_NTSC_RGB_OUT( x, rgb_out, bits ) {\
	apple_ntsc_rgb_t raw_ =\
		kernel0  [x  ] + kernel1  [(x+3)%7+14] +\
		kernelx0 [x+7] + kernelx1 [(x+3)%7+21];\
	APPLE_NTSC_CLAMP_( raw_, 0 );\
	APPLE_NTSC_RGB_OUT_( rgb_out, bits, 0 );\
}

/* private */
enum { apple_ntsc_entry_size = 14 * 2 };
typedef unsigned long apple_ntsc_rgb_t;
struct apple_ntsc_t {
	apple_ntsc_rgb_t table [apple_ntsc_palette_size] [apple_ntsc_entry_size];
};

#define APPLE_NTSC_ENTRY_( ntsc, n ) (ntsc)->table [n & 0x0F]

/* common ntsc macros */
#define apple_ntsc_rgb_builder    ((1L << 21) | (1 << 11) | (1 << 1))
#define apple_ntsc_clamp_mask     (apple_ntsc_rgb_builder * 3 / 2)
#define apple_ntsc_clamp_add      (apple_ntsc_rgb_builder * 0x101)
#define APPLE_NTSC_CLAMP_( io, shift ) {\
	apple_ntsc_rgb_t sub = (io) >> (9-(shift)) & apple_ntsc_clamp_mask;\
	apple_ntsc_rgb_t clamp = apple_ntsc_clamp_add - sub;\
	io |= clamp;\
	clamp -= sub;\
	io &= clamp;\
}

#define APPLE_NTSC_COLOR_IN_( index, color, ENTRY, table ) {\
	unsigned color_;\
	kernelx##index = kernel##index;\
	kernel##index = (color_ = (color), ENTRY( table, color_ ));\
}

/* x is always zero except in snes_ntsc library */
#define APPLE_NTSC_RGB_OUT_( rgb_out, bits, x ) {\
	if ( bits == 16 )\
		rgb_out = (raw_>>(13-x)& 0xF800)|(raw_>>(8-x)&0x07E0)|(raw_>>(4-x)&0x001F);\
	if ( bits == 24 || bits == 32 )\
		rgb_out = (raw_>>(5-x)&0xFF0000)|(raw_>>(3-x)&0xFF00)|(raw_>>(1-x)&0xFF);\
	if ( bits == 15 )\
		rgb_out = (raw_>>(14-x)& 0x7C00)|(raw_>>(9-x)&0x03E0)|(raw_>>(4-x)&0x001F);\
	if ( bits == 0 )\
		rgb_out = raw_ << x;\
}

#ifdef __cplusplus
	}
#endif

#endif
