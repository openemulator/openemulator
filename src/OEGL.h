
/**
 * OpenEmulator
 * OpenEmulator OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator OpenGL interface
 */

#ifndef _OEGL_H
#define _OEGL_H

#include <pthread.h>
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
	float x;
	float y;
} OEGLPoint;

typedef struct
{
	float width;
	float height;
} OEGLSize;

typedef struct
{
	OEGLPoint origin;
	OEGLSize size;
} OEGLRect;

class OEGL
{
public:
	OEGL();
	~OEGL();
	
	void update(HostVideoScreens *screens);
	void draw(int width, int height);
	
private:
	GLuint texture[OEGL_TEX_NUM];
	pthread_mutex_t glMutex;
	
	OEGLSize windowSize;
	float windowAspectRatio;
	bool windowRedraw;
	
	HostVideoScreens screens;
	int columnNum;
	int rowNum;
	OEGLSize cellSize;
	
	OEGLSize getScreenSize(HostVideoConfiguration *conf);
	
	void updateCellSize();
	void updateScreenMatrix();
	
	void drawScreen(HostVideoScreen *screen, int index);

	void renderScreen(OEUInt32 *frambuffer, OEGLSize framebufferSize,
					  OEGLRect frame);
};

#endif
