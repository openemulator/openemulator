
/**
 * libntsc
 * Useful signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements useful signal processing functions.
 */

#include <math.h>
#include <stdlib.h>

#include "ntsc.h"
#include "ntscSignalProcessing.h"

double ntscUPhase[] = {M_SQRT2, 0, -M_SQRT2, 0};
double ntscVPhase[] = {0, -M_SQRT2, 0, M_SQRT2};

//
// Based on code at:
// http://nolandda.org/pusite/cs490-dsp/dft.c.html
//
void calculateRealIDFT(double *w, unsigned int n)
{
	double *x = malloc(sizeof(double) * n);
	if (!x)
		return;
	
	for (int i = 0; i < n; i++)
	{
		x[i] = 0.0;
		
		double cosArg = 2.0 * M_PI * i / n;
		for (int j = 0; j < n; j++)
			x[i] += w[j] * cos(j * cosArg);
	}
	
	for (int i = 0; i < n; i++)
		w[i] = x[i] / n;
	
	free(x);
	
	return;
}

void calculateLanczosWindow(double *w, unsigned int n, double fc)
{
	int halfN = n / 2;
	for (int i = 0; i < n; i++)
	{
		double x = (i - halfN) * fc;
		
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
void calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb)
{
	int m = n - 1;
	double alpha = sidelobeDb / 20.0;
	double beta = cosh(acosh(pow(10, alpha)) / m);
	
	for (int i = 0; i < m; i++)
	{
		double a = fabs(beta * cos(M_PI * i / m));
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

void multiplyWindow(double *x, double *w, unsigned int n)
{
	for (int i = 0; i < n; i++)
		x[i] *= w[i];
}

void normalizeWindow(double *w, unsigned int n)
{
	double sum = 0.0;
	for (int i = 0; i < n; i++)
		sum += w[i];
	
	double gain = 1.0 / sum;
	for (int i = 0; i < n; i++)
		w[i] *= gain;
}

void copyDecoderMatrix(double *to, const double *from)
{
	for (int i = 0; i < NTSC_DECODERMATRIX_SIZE; i++)
		*to++ = *from++;
}

void transformDecoderMatrix(double *m, double saturation, double hue)
{
	double cosArg = cos(M_PI * hue) * saturation;
	double sinArg = sin(M_PI * hue) * saturation;
	
	for (int i = 0; i < NTSC_DECODERMATRIX_DIM; i++)
	{
		double u = m[1];
		double v = m[2];
		
		m[1] = u * cosArg - v * sinArg;
		m[2] = u * sinArg + v * cosArg;
		
		m += NTSC_DECODERMATRIX_DIM;
	}
}

void applyDecoderMatrix(double *rgb, double *yuv, double *m)
{
	*rgb++ = m[0] * yuv[0] + m[1] * yuv[1] + m[2] * yuv[2];
	*rgb++ = m[3] * yuv[0] + m[4] * yuv[1] + m[5] * yuv[2];
	*rgb++ = m[6] * yuv[0] + m[7] * yuv[1] + m[8] * yuv[2];
}

void applyOffsetAndGain(double *rgb, double offset, double gain)
{
	*rgb++ = offset + *rgb * gain;
	*rgb++ = offset + *rgb * gain;
	*rgb++ = offset + *rgb * gain;
}
