
/**
 * OpenEmulator
 * OpenEmulator OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator OpenGL interface
 */

#include <math.h>

#include "OEGL.h"

inline OEGLPoint OEGLMakePoint(float x, float y)
{
	OEGLPoint p;
	p.x = x;
	p.y = y;
	return p;
}

inline OEGLSize OEGLMakeSize(float w, float h)
{
	OEGLSize s;
	s.width = w;
	s.height = h;
	return s;
}

inline OEGLRect OEGLMakeRect(float x, float y, float w, float h)
{
	OEGLRect r;
	r.origin.x = x;
	r.origin.y = y;
	r.size.width = w;
	r.size.height = h;
	return r;
}

inline float OEGLRatio(OEGLSize s)
{
	return s.width ? (s.width / s.height) : 1.0F;
}

inline float OEGLMinX(OEGLRect r)
{
	return r.origin.x;
}

inline float OEGLMaxX(OEGLRect r)
{
	return r.origin.x + r.size.width;
}

inline float OEGLMinY(OEGLRect r)
{
	return r.origin.y;
}

inline float OEGLMaxY(OEGLRect r)
{
	return r.origin.y + r.size.height;
}

OEGL::OEGL()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0, 0.0, 0.0, 0.5);
	
	glGenTextures(OEGL_TEX_NUM, texture);
	
	pthread_mutex_init(&glMutex, NULL);
	
	windowSize = OEGLMakeSize(0, 0);
	
	columnNum = 1;
	rowNum = 1;
	cellSize = OEGLMakeSize(0, 0);
}

OEGL::~OEGL()
{
	glDeleteTextures(OEGL_TEX_NUM, texture);
	
	pthread_mutex_destroy(&glMutex);
}

void OEGL::update(HostVideoScreens *screens)
{
	pthread_mutex_lock(&glMutex);
	
	this->screens = *screens;
	updateCellSize();
	updateScreenMatrix();
	
	windowRedraw = true;
	
	pthread_mutex_unlock(&glMutex);
}

void OEGL::draw(int width, int height)
{
	pthread_mutex_lock(&glMutex);
	
	glViewport(0, 0, width, height);
	
	if ((width != windowSize.width) || (height != windowSize.height))
	{
		windowSize = OEGLMakeSize(width, height);
		windowAspectRatio = OEGLRatio(windowSize);
		windowRedraw = true;
	}
	
	if (windowRedraw)
		glClear(GL_COLOR_BUFFER_BIT);
	
	int screenIndex = 0;
	for (HostVideoScreens::iterator i = screens.begin();
		 i != screens.end();
		 i++)
	{
		drawScreen(*i, screenIndex);
		screenIndex++;
	}
	
	windowRedraw = false;
	
	pthread_mutex_unlock(&glMutex);
}

OEGLSize OEGL::getScreenSize(HostVideoConfiguration *conf)
{
	return OEGLMakeSize(conf->paddingLeft +	conf->contentWidth +
						conf->paddingRight,
						conf->paddingTop + conf->contentHeight +
						conf->paddingBottom);
}

void OEGL::updateCellSize()
{
	cellSize = OEGLMakeSize(0, 0);
	
	for (HostVideoScreens::iterator i = screens.begin();
		 i != screens.end();
		 i++)
	{
		OEGLSize size = getScreenSize(&(*i)->conf);
		
		if (size.width > cellSize.width)
			cellSize.width = size.width;
		if (size.height > cellSize.height)
			cellSize.height = size.height;
	}
}

void OEGL::updateScreenMatrix()
{
	int screenNum = screens.size();
	int value;
	
	value = (int) ceil(sqrt(screenNum));
	columnNum = value ? value : 1;
	value = (int) ceil(screenNum / columnNum);
	rowNum = value ? value : 1;
}

void OEGL::drawScreen(HostVideoScreen *screen, int index)
{
	HostVideoConfiguration *conf = &(screen->conf);
	
	// Calculate OpenGL frame coords
	int x = index % columnNum;
	int y = index / columnNum;
	
	OEGLRect frame = OEGLMakeRect(2.0F * x / columnNum - 1.0F,
								  2.0F * y / rowNum - 1.0F,
								  2.0F / columnNum,
								  2.0F / rowNum);
	
	// Correct aspect ratio
	OEGLSize screenSize = getScreenSize(conf);
	float screenAspectRatio = OEGLRatio(screenSize);
	
	if (screenAspectRatio < windowAspectRatio)
	{
		float ratio = screenAspectRatio / windowAspectRatio;
		
		frame.origin.x += (1.0F - ratio) * frame.size.width / 2.0F;
		frame.size.width *= ratio;
	}
	else
	{
		float ratio = windowAspectRatio / screenAspectRatio;
		
		frame.origin.y += (1.0F - ratio) * frame.size.height / 2.0F;
		frame.size.height *= ratio;
	}
	
	// Correct padding
	frame.origin.x += conf->paddingLeft / screenSize.width * frame.size.width;
	frame.origin.y += conf->paddingTop / screenSize.height * frame.size.height;
	frame.size.width *= conf->contentWidth / screenSize.width;
	frame.size.height *= conf->contentHeight / screenSize.height;
	
	// Draw
	OEGLSize framebufferSize = OEGLMakeSize(conf->framebufferWidth,
											conf->framebufferHeight);
	OEUInt32 *framebuffer = screen->framebuffer[0];	
	
	renderScreen(framebuffer, framebufferSize, frame);
}

void OEGL::renderScreen(OEUInt32 *framebuffer, OEGLSize framebufferSize, OEGLRect frame)
{
	// Upload texture
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[OEGL_TEX_FRAME]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGB, framebufferSize.width, framebufferSize.height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
	
	// Render quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(OEGLMinX(frame), OEGLMaxY(frame));
	glTexCoord2f(framebufferSize.width, 0);
	glVertex2f(OEGLMaxX(frame), OEGLMaxY(frame));
	glTexCoord2f(framebufferSize.width, framebufferSize.height);
	glVertex2f(OEGLMaxX(frame), OEGLMinY(frame));
	glTexCoord2f(0, framebufferSize.height);
	glVertex2f(OEGLMinX(frame), OEGLMinY(frame));
	glEnd();
}
