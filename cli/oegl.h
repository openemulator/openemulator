
/**
 * OpenEmulator
 * OpenEmulator/OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/OpenGL interface
 */

#ifndef _OEGL_H
#define _OEGL_H

#include <OpenGL/OpenGL.h>

#include "Host.h"

enum 
{
	OEGL_TEX_FRAME,
	OEGL_TEX_BEZELPOWER,
	OEGL_TEX_BEZELPAUSE,
	OEGL_TEX_BEZELCAPTURE,
	OEGL_TEX_NUM,
};

typedef struct
{
	GLuint texture[OEGL_TEX_NUM];
} OEGLContext;

OEGLContext *oeglOpen();
void oeglClose(OEGLContext *context);
void oeglUpdate(OEGLContext *context, void *ref);
void oeglDraw(OEGLContext *context, int width, int height);

#endif
