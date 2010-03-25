
/**
 * libemulator
 * Common signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements common signal processing functions
 */

void calculateRealDFT(double *w, unsigned int m);
void calculateLanczosWindow(double *w, unsigned int m, double fc);
void calculateChebyshevWindow(double *w, unsigned int m, double sidelobeDb);
void applyWindow(double *x, double *w, unsigned int m);
void normalizeWindow(double *w, unsigned int m);
