
/**
 * libntsc
 * Useful signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements useful signal processing functions.
 */

int calculateRealIDFT(double *w, unsigned int n);
void calculateLanczosWindow(double *w, unsigned int n, double fc);
int calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb);
void applyWindow(double *x, double *w, unsigned int n);
void normalizeWindow(double *w, unsigned int n);
