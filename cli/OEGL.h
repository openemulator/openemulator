
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

class OEGL
{
public:
	OEGL();
	~OEGL();
	
	void update(HostVideoScreens *screens);
	void draw(int width, int height);
	
	private:
		GLuint texture[OEGL_TEX_NUM];
};

#endif
