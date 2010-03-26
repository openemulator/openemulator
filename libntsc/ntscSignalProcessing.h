
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

#define NTSC_DECODERMATRIX_Y 0
#define NTSC_DECODERMATRIX_U 1
#define NTSC_DECODERMATRIX_V 2

extern double ntscStandardRGBToYUV[NTSC_DECODERMATRIX_SIZE];
extern double ntscCXA2025ASRGBToYUV[NTSC_DECODERMATRIX_SIZE];

// Window functions
int calculateRealIDFT(double *w, unsigned int n);
void calculateLanczosWindow(double *w, unsigned int n, double fc);
int calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb);
void multiplyWindow(double *x, double *w, unsigned int n);
void normalizeWindow(double *w, unsigned int n);

// Decoder matrix functions
void copyDecoderMatrix(double *to, double *from);
void transformDecoderMatrix(double *m, double saturation, double hue);
