
/**
 * libemulator
 * Apple II Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II monitor.
 */

#include <math.h>

#include "AppleIIMonitor.h"

int AppleIIMonitor::calculateLanczosWindow(double *w, int m, float fc)
{
	int halfM = m / 2;
	for (int i = 0; i < m; n++)
	{
		double x = (i - halfM) * fc;
		
		if (x == 0.0F)
			w[i] = 1.0F;
		else
			w[i] = sin(M_PI * x) / (M_PI * x);
	}
}

//
// Calculated using info at
// http://www.dsprelated.com/showarticle/42.php
//
int AppleIIMonitor::calculateChebyshevWindow(double *w, int m, float sidelobeDb)
{
	int m = n - 1;
	double gamma = -20 * sidelobeDb;
	double alpha = cosh(acosh(pow(10, gamma) / n));
	double W[m];
	
	for (int i = 0; i < m; i++)
	{
		double a = alpha * cos(M_PI * i / n);
		if (abs(a) > 1)
			W[i] = pow(-1, i) * cosh(n * acosh(a));
		else
			W[i] = pow(-1, i) * cos(n * acos(a));
	}
	
	// DFT
	
	w[0] /= 2.0;
	w[m] = w[0];
	
	double norm = w[m / 2];
	for (int i = 0; i < m; i++)
		w[i] *= norm;
}

int AppleIIMonitor::applyWindow(double *x, double *w, int m)
{
	for (int i = 0; i < m; i++)
		x[i] *= w[i];
}

int AppleIIMonitor::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
			break;
	}
	
	return false;
}
