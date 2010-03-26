
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

#include "signalProcessing.h"

//
// Based on code at:
// http://nolandda.org/pusite/cs490-dsp/dft.c.html
//
int calculateRealIDFT(double *w, unsigned int n)
{
	double *x = malloc(sizeof(double) * n);
	if (!x)
		return 0;
	
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
	
	return 1;
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
int calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb)
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
	
	if (!calculateRealIDFT(w, m))
		return 0;
	
	w[0] /= 2.0;
	w[m] = w[0];
	
	double norm = 1.0 / w[m / 2];
	for (int i = 0; i < m; i++)
		w[i] *= norm;
	
	return 1;
}

void applyWindow(double *x, double *w, unsigned int n)
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
