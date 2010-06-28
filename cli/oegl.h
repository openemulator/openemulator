
/**
 * OpenEmulator
 * OpenEmulator/OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/OpenGL interface.
 */

#ifndef _OEGL_H
#define _OEGL_H

typedef void (*OEGLSetContext)(void *emulation);

// User interface
void oeglOpen(OEGLSetContext setContext);
void oeglClose();
void oeglInit(void *emulation);
void oeglDraw(void *emulation);

#endif
