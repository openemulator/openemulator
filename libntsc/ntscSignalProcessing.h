
/**
 * libntsc
 * Useful signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements useful signal processing functions.
 */

#define NTSC_COLORGAIN		255.0
#define NTSC_COLOROFFSET	256.5

#define NTSC_PHASENUM		4
#define NTSC_PHASEMASK		(NTSC_PHASENUM - 1)

#define NTSC_CLAMP_ANDMASK	(1 << 28 | 1 << 18 | 1 << 8)
#define NTSC_CLAMP_ORMASK	(1 << 29 | 1 << 19 | 1 << 9)

// Macros
#define NTSC_PACK(r, g, b)\
(((int) r << 20) | ((int) g << 10) | ((int) b))

#define NTSC_UNPACK(value)\
(value >> 4 & 0xff0000) | (value >> 2 & 0x00ff00) | (value & 0x0000ff)

#define NTSC_CLAMP(value)\
{\
int andMask = (value & NTSC_CLAMP_ANDMASK);\
int orMask = (value & NTSC_CLAMP_ORMASK);\
andMask -= (andMask >> 8);\
orMask -= (orMask >> 9);\
value &= andMask;\
value |= orMask;\
}

extern double ntscUPhase[NTSC_PHASENUM];
extern double ntscVPhase[NTSC_PHASENUM];

// Window functions
void calculateRealIDFT(double *w, unsigned int n);
void calculateLanczosWindow(double *w, unsigned int n, double fc);
void calculateChebyshevWindow(double *w, unsigned int n, double sidelobeDb);
void multiplyWindow(double *x, double *w, unsigned int n);
void normalizeWindow(double *w, unsigned int n);

// RGB functions
void copyDecoderMatrix(double *to, const double *from);
void transformDecoderMatrix(double *m, double saturation, double hue);
void applyDecoderMatrix(double *rgb, double *yuv, double *m);
void applyOffsetAndGain(double *rgb, double offset, double gain);
