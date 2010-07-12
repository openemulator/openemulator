
/**
 * OpenEmulator
 * OpenEmulator/OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/OpenGL interface
 */

#include <stdio.h>

#include "oegl.h"

OEGLContext *oeglOpen()
{
	OEGLContext *context = new OEGLContext;
	if (!context)
		return NULL;
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0, 0.0, 0.0, 0.5);
	
	// Prepare for textures
	glGenTextures(OEGL_TEX_NUM, context->texture);
	
	return context;
}

void oeglClose(OEGLContext *context)
{
	if (context)
		glDeleteTextures(OEGL_TEX_NUM, context->texture);
}

void oeglUpdate(OEGLContext *context, void *ref)
{
}

void oeglDraw(OEGLContext *context, int width, int height)
{
	int framebufferWidth = 560;
	int framebufferHeight = 384;
	int contentWidth = 560;
	int contentHeight = 413;
	int screenWidth = 702;
	int screenHeight = 413;
	
	glViewport(0, 0, width, height);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (!context)
		return;
	
	// Upload texture
	int *framebuffer = new int[framebufferWidth * framebufferHeight];
	int *p = framebuffer;
	
	for (int y = 0; y < framebufferHeight; y++)
	{
		float kk = 1;
		
		for (int x = 0; x < framebufferWidth; x++)
		{
			float l;
//			float l = (((x >> 0) & 0x1) ^ ((y >> 1) & 0x1)) * 1.0;
			kk = 0.99 * kk;
			l = kk;
			
			if (y % 2)
				l *= 0.5;
			
			int r = l * 0x33;
			int g = l * 0xcc;
			int b = l * 0x44;
			*p++ = (b << 16) | (g << 8) | r;
		}
	}
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, context->texture[OEGL_TEX_FRAME]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGB, framebufferWidth, framebufferHeight,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
	
	delete framebuffer;
	
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-0.8, 0.8);
	glTexCoord2f(framebufferWidth, 0);
	glVertex2f(0.8, 0.8);
	glTexCoord2f(framebufferWidth, framebufferHeight);
	glVertex2f(0.8, -0.8);
	glTexCoord2f(0, framebufferHeight);
	glVertex2f(-0.8, -0.8);
	glEnd();
}
