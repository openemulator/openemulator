/* apple_ntsc 0.1.0. http://www.slack.net/~ant/ */

#include "apple_ntsc.h"

/* Copyright (C) 2006-2007 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

apple_ntsc_setup_t const apple_ntsc_monochrome = { 0,-1, 0, 0,.2,  0, .2,-.2,-.2,-1, 0,  0 };
apple_ntsc_setup_t const apple_ntsc_composite  = { 0, 0, 0, 0, 0,  0,.15,  0,  0, 0, 0,  0 };
apple_ntsc_setup_t const apple_ntsc_svideo     = { 0, 0, 0, 0, 0,  0,.25, -1, -1, 0, 0,  0 };
apple_ntsc_setup_t const apple_ntsc_rgb        = { 0, 0, 0, 0,.2,  0,.70, -1, -1,-1, 0,  0 };

#define alignment_count 2
#define burst_count     1
#define rescale_in      8
#define rescale_out     7

#define artifacts_mid   0.6f
#define fringing_mid    0.6f
#define std_decoder_hue -135
/*#define DISABLE_CORRECTION 1 */

#include "apple_ntsc_impl.h"

/* 2 input pixels -> 8 composite samples */
pixel_info_t const apple_ntsc_pixels [alignment_count] = {
	{ PIXEL_OFFSET( -4, -9 ), { 1, 1, 1, 1 } },
	{ PIXEL_OFFSET(  0, -5 ), { 1, 1, 1, 1 } },
};

static float const apple_ntsc_factor = 0.95f;

static void gen_kernel2( init_t* impl, apple_ntsc_rgb_t* out )
{
	int i;
	float lookup [3];
	lookup [0] = 0;
	lookup [1] = apple_ntsc_factor * impl->contrast + impl->brightness - rgb_offset;
	lookup [2] = apple_ntsc_factor * rgb_unit;
	
	for ( i = 0; i < apple_ntsc_palette_size * alignment_count; i++ )
	{
		float yc0 = lookup [i >> 4 & 1];
		float yc1 = lookup [i >> 3 & 1];
		float yc2 = lookup [i >> 2 & 1];
		float yc3 = lookup [i >> 1 & 1];
		
		float ic0 =  lookup [i >> 3 & 2];
		float qc1 =  lookup [i >> 2 & 2];
		float ic2 = -lookup [i >> 1 & 2];
		float qc3 = -lookup [i >> 0 & 2];
		
		float const* k = &impl->kernel [apple_ntsc_pixels [i & 1].offset];
		int n;
		
		float temp;
		
		temp = (ic0 + ic2) * 0.5f;
		ic0 -= (ic0 - temp) * (1 - impl->fringing);
		ic2 -= (ic2 - temp) * (1 - impl->fringing);
		
		temp = (qc1 + qc3) * 0.5f;
		qc1 -= (qc1 - temp) * (1 - impl->fringing);
		qc3 -= (qc3 - temp) * (1 - impl->fringing);
		
		temp = (yc0 + yc1 + yc2 + yc3) * 0.25f;
		yc0 -= (yc0 - temp) * (1 - impl->artifacts);
		yc1 -= (yc1 - temp) * (1 - impl->artifacts);
		yc2 -= (yc2 - temp) * (1 - impl->artifacts);
		yc3 -= (yc3 - temp) * (1 - impl->artifacts);
		
		for ( n = rgb_kernel_size; n; --n )
		{
			float i = k[0]*ic0 + k[2]*ic2;
			float q = k[1]*qc1 + k[3]*qc3;
			float y = k[kernel_size+0]*yc0 + k[kernel_size+1]*yc1 +
			          k[kernel_size+2]*yc2 + k[kernel_size+3]*yc3 + rgb_offset;
			if ( rescale_out <= 1 )
				k--;
			else if ( k >= &impl->kernel [kernel_size * 2 * (rescale_out - 1)] )
				k -= kernel_size * 2 * (rescale_out - 1) + 2;
			else
				k += kernel_size * 2 - 1;
			{
				int r, g, b = YIQ_TO_RGB( y, i, q, impl->to_rgb, int, r, g );
				*out++ = PACK_RGB( r, g, b ) - rgb_bias;
			}
		}
	}
}

static void correct_errors( apple_ntsc_rgb_t color, apple_ntsc_rgb_t* out )
{
	unsigned i;
	for ( i = 0; i < rgb_kernel_size / 2; i++ )
	{
		apple_ntsc_rgb_t error = color -
				out [i    ] - out [(i+3)%7+14] -
				out [i + 7] - out [(i+3)%7+21];
		DISTRIBUTE_ERROR( (i+3)%7+14, i + 7, (i+3)%7+21 );
	}
}

static float const yiq_palette [apple_ntsc_palette_size] [3] = {
	{ 0.00f, 0.00f, 0.00f},
	{ 0.25f, 0.00f,-0.50f},
	{ 0.25f,-0.50f, 0.00f},
	{ 0.50f,-0.50f,-0.50f},
	{ 0.25f, 0.00f, 0.50f},
	{ 0.50f, 0.00f, 0.00f},
	{ 0.50f,-0.50f, 0.50f},
	{ 0.75f,-0.50f, 0.00f},
	{ 0.25f, 0.50f, 0.00f},
	{ 0.50f, 0.50f,-0.50f},
	{ 0.50f, 0.00f, 0.00f},
	{ 0.75f, 0.00f,-0.50f},
	{ 0.50f, 0.50f, 0.50f},
	{ 0.75f, 0.50f, 0.00f},
	{ 0.75f, 0.00f, 0.50f},
	{ 1.00f, 0.00f, 0.00f},
};

void apple_ntsc_init( apple_ntsc_t* ntsc, apple_ntsc_setup_t const* setup )
{
	init_t impl;
	if ( !setup )
		setup = &apple_ntsc_composite;
	init( &impl, setup );
	
	{
		int entry;
		unsigned char* palette_out = setup->palette_out;
		apple_ntsc_rgb_t* kernel_out = (ntsc ? ntsc->table [0] : 0);
		if ( kernel_out )
			gen_kernel2( &impl, kernel_out ); /* generates all kernels at once */
		
		for ( entry = 0; entry < apple_ntsc_palette_size; entry++ )
		{
			float y = yiq_palette [entry] [0] * apple_ntsc_factor;
			float i = yiq_palette [entry] [1];
			float q = yiq_palette [entry] [2];
			
			y = y * impl.contrast + impl.brightness;
			i *= rgb_unit * apple_ntsc_factor;
			q *= rgb_unit * apple_ntsc_factor;
			
			{
				int r, g, b = YIQ_TO_RGB( y, i, q, impl.to_rgb, int, r, g );
				apple_ntsc_rgb_t rgb = PACK_RGB( r, g, b );
				
				if ( palette_out )
				{
					RGB_PALETTE_OUT( rgb, palette_out );
					palette_out += 3;
				}
				
				if ( kernel_out )
				{
					correct_errors( rgb, kernel_out );
					kernel_out += apple_ntsc_entry_size;
				}
			}
		}
	}
}

#ifndef APPLE_NTSC_NO_BLITTERS

void apple_ntsc_blit( apple_ntsc_t const* ntsc, APPLE_NTSC_IN_T const* input, long in_row_width,
		int in_width, int height, void* rgb_out, long out_pitch )
{
	int const chunk_count = in_width / apple_ntsc_in_chunk;
	while ( height-- )
	{
		APPLE_NTSC_IN_T const* line_in = input;
		APPLE_NTSC_BEGIN_ROW( ntsc, apple_ntsc_black, apple_ntsc_black );
		apple_ntsc_out_t* restrict line_out = (apple_ntsc_out_t*) rgb_out;
		int n;
		
		for ( n = chunk_count; n; --n )
		{
			/* order of input and output pixels must not be altered */
			APPLE_NTSC_COLOR_IN( 0, ntsc, APPLE_NTSC_ADJ_IN( line_in [0] ) );
			APPLE_NTSC_RGB_OUT( 0, line_out [0], APPLE_NTSC_OUT_DEPTH );
			APPLE_NTSC_RGB_OUT( 1, line_out [1], APPLE_NTSC_OUT_DEPTH );
			APPLE_NTSC_RGB_OUT( 2, line_out [2], APPLE_NTSC_OUT_DEPTH );
			APPLE_NTSC_RGB_OUT( 3, line_out [3], APPLE_NTSC_OUT_DEPTH );
			
			APPLE_NTSC_COLOR_IN( 1, ntsc, APPLE_NTSC_ADJ_IN( line_in [1] ) );
			APPLE_NTSC_RGB_OUT( 4, line_out [4], APPLE_NTSC_OUT_DEPTH );
			APPLE_NTSC_RGB_OUT( 5, line_out [5], APPLE_NTSC_OUT_DEPTH );
			APPLE_NTSC_RGB_OUT( 6, line_out [6], APPLE_NTSC_OUT_DEPTH );
			
			line_in  += 2;
			line_out += 7;
		}
		
		/* finish final pixels */
		APPLE_NTSC_COLOR_IN( 0, ntsc, apple_ntsc_black );
		APPLE_NTSC_RGB_OUT( 0, line_out [0], APPLE_NTSC_OUT_DEPTH );
		APPLE_NTSC_RGB_OUT( 1, line_out [1], APPLE_NTSC_OUT_DEPTH );
		APPLE_NTSC_RGB_OUT( 2, line_out [2], APPLE_NTSC_OUT_DEPTH );
		APPLE_NTSC_RGB_OUT( 3, line_out [3], APPLE_NTSC_OUT_DEPTH );
		
		APPLE_NTSC_COLOR_IN( 1, ntsc, apple_ntsc_black );
		APPLE_NTSC_RGB_OUT( 4, line_out [4], APPLE_NTSC_OUT_DEPTH );
		APPLE_NTSC_RGB_OUT( 5, line_out [5], APPLE_NTSC_OUT_DEPTH );
		APPLE_NTSC_RGB_OUT( 6, line_out [6], APPLE_NTSC_OUT_DEPTH );
		
		input += in_row_width;
		rgb_out = (char*) rgb_out + out_pitch;
	}
}

#endif
