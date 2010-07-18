
/**
 * libcomposite
 * Generic signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements generic signal processing functions.
 */

#include <math.h>
#include <stdlib.h>

#include "composite.h"
#include "compositeSignalProcessing.h"

float compositeUPhase[] = {M_SQRT2, 0, -M_SQRT2, 0};
float compositeVPhase[] = {0, -M_SQRT2, 0, M_SQRT2};

//
// Based on code at:
// http://nolandda.org/pusite/cs490-dsp/dft.c.html
//
void calculateRealIDFT(float *w, unsigned int n)
{
	float *x = malloc(sizeof(float) * n);
	if (!x)
		return;
	
	for (int i = 0; i < n; i++)
	{
		x[i] = 0.0;
		
		float cosArg = 2.0 * M_PI * i / n;
		for (int j = 0; j < n; j++)
			x[i] += w[j] * cos(j * cosArg);
	}
	
	for (int i = 0; i < n; i++)
		w[i] = x[i] / n;
	
	free(x);
	
	return;
}

void calculateLanczosWindow(float *w, unsigned int n, float fc)
{
	int halfN = n / 2;
	for (int i = 0; i < n; i++)
	{
		float x = (i - halfN) * fc;
		
		if (x == 0.0F)
			w[i] = 1.0F;
		else
			w[i] = sin(M_PI * x) / (M_PI * x);
	}
}

//
// Based on ideas at:
// http://www.dsprelated.com/showarticle/42.php
//
void calculateChebyshevWindow(float *w, unsigned int n, float sidelobeDb)
{
	int m = n - 1;
	float alpha = sidelobeDb / 20.0;
	float beta = cosh(acosh(pow(10, alpha)) / m);
	
	for (int i = 0; i < m; i++)
	{
		float a = fabs(beta * cos(M_PI * i / m));
		if (a > 1)
			w[i] = pow(-1, i) * cosh(m * acosh(a));
		else
			w[i] = pow(-1, i) * cos(m * acos(a));
	}
	
	calculateRealIDFT(w, m);
	
	w[0] /= 2.0;
	w[m] = w[0];
	
	return;
}

void multiplyWindow(float *x, float *w, unsigned int n)
{
	for (int i = 0; i < n; i++)
		x[i] *= w[i];
}

void normalizeWindow(float *w, unsigned int n)
{
	float sum = 0.0;
	for (int i = 0; i < n; i++)
		sum += w[i];
	
	float gain = 1.0 / sum;
	for (int i = 0; i < n; i++)
		w[i] *= gain;
}

void copyDecoderMatrix(float *to, const float *from)
{
	for (int i = 0; i < COMPOSITE_DECODERMATRIX_SIZE; i++)
		*to++ = *from++;
}

void transformDecoderMatrix(float *m, float saturation, float hue)
{
	float cosArg = cos(M_PI * hue) * saturation;
	float sinArg = sin(M_PI * hue) * saturation;
	
	for (int i = 0; i < COMPOSITE_DECODERMATRIX_DIM; i++)
	{
		float u = m[1];
		float v = m[2];
		
		m[1] = u * cosArg - v * sinArg;
		m[2] = u * sinArg + v * cosArg;
		
		m += COMPOSITE_DECODERMATRIX_DIM;
	}
}

void applyDecoderMatrix(float *rgb, float *yuv, float *m)
{
	*rgb++ = m[0] * yuv[0] + m[1] * yuv[1] + m[2] * yuv[2];
	*rgb++ = m[3] * yuv[0] + m[4] * yuv[1] + m[5] * yuv[2];
	*rgb++ = m[6] * yuv[0] + m[7] * yuv[1] + m[8] * yuv[2];
}

void applyOffsetAndGain(float *rgb, float offset, float gain)
{
	*rgb++ = offset + *rgb * gain;
	*rgb++ = offset + *rgb * gain;
	*rgb++ = offset + *rgb * gain;
}

//
// Efficient RGB mixing based on:
// http://www.fly.net/~ant/info/rgb_mixing.html
//
void interpolateScanlines(int *buffer, int width, int height, int pitch)
{
	int line = width + pitch;
	height--;
	
	while (height--)
	{
		int *p = buffer + line;
		
		for (int n = width; n; n--)
		{
			int x = *(p - line);
			int y = *(p + line);
			int m = (x + y - ((x ^ y) & 0x010101)) >> 1;
			*p++ = m - ((m & 0xf0f0f0) >> 4);
		}
		
		buffer += 2 * line;
	}
}
