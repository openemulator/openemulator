
/**
 * libntsc
 * Useful signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements useful signal processing functions.
 */

#define NTSC_DECODERMATRIX_DIM 3
#define NTSC_DECODERMATRIX_SIZE 9

extern double ntscStandardRGBToYUV[NTSC_DECODERMATRIX_SIZE];
extern double ntscCXA2025ASRGBToYUV[NTSC_DECODERMATRIX_SIZE];
extern double ntscUPhase[8];
extern double ntscVPhase[8];

// Window functions
int calculateRealIDFT(double *w, unsigned int n);
void calculateLanczosWindow(double *w, unsigned int n, double fc);
int calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb);
void multiplyWindow(double *x, double *w, unsigned int n);
void normalizeWindow(double *w, unsigned int n);

// RGB functions
void copyDecoderMatrix(double *to, double *from);
void transformDecoderMatrix(double *m, double saturation, double hue);
void applyDecoderMatrix(double *rgb, double *yuv, double *m);
void applyGainAndOffset(double *rgb, double gain, double offset);
