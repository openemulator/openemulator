
/**
 * libcomposite
 * Generic signal processing functions
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements generic signal processing functions.
 */

#define COMPOSITE_DECODERMATRIX_DIM		3
#define COMPOSITE_DECODERMATRIX_SIZE	9

#define COMPOSITE_COLORGAIN		255.0F
#define COMPOSITE_COLOROFFSET	256.5F

#define COMPOSITE_PHASENUM		4
#define COMPOSITE_PHASEMASK		(COMPOSITE_PHASENUM - 1)

#define COMPOSITE_CLAMP_ANDMASK	(1 << 28 | 1 << 18 | 1 << 8)
#define COMPOSITE_CLAMP_ORMASK	(1 << 29 | 1 << 19 | 1 << 9)

// Macros
#define COMPOSITE_PACK(r, g, b)\
((((int) r & 0x3ff) << 20) | (((int) g & 0x3ff) << 10) | ((int) b & 0x3ff))

#define COMPOSITE_UNPACK(value)\
(value >> 4 & 0xff0000) | (value >> 2 & 0x00ff00) | (value & 0x0000ff)

#define COMPOSITE_CLAMP(value)\
{\
int andMask = (value & COMPOSITE_CLAMP_ANDMASK);\
int orMask = (value & COMPOSITE_CLAMP_ORMASK);\
andMask -= (andMask >> 8);\
orMask -= (orMask >> 9);\
value &= andMask;\
value |= orMask;\
}

extern float compositeUPhase[COMPOSITE_PHASENUM];
extern float compositeVPhase[COMPOSITE_PHASENUM];

// Window functions
void calculateRealIDFT(float *w, unsigned int n);
void calculateLanczosWindow(float *w, unsigned int n, float fc);
void calculateChebyshevWindow(float *w, unsigned int n, float sidelobeDb);
void multiplyWindow(float *x, float *w, unsigned int n);
void normalizeWindow(float *w, unsigned int n);

// RGB functions
void copyDecoderMatrix(float *to, const float *from);
void transformDecoderMatrix(float *m, float saturation, float hue);
void applyDecoderMatrix(float *rgb, float *yuv, float *m);
void applyOffsetAndGain(float *rgb, float offset, float gain);
