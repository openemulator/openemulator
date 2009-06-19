
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <OpenGL/gl.h>

#import "DocumentView.h"

@implementation DocumentView

- (id)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute pixelFormatAtrributes[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat;
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAtrributes];
	
	self = [super initWithFrame:frame pixelFormat:pixelFormat];
	if(self)
	{
		int bitmap[560 * 192];
		
		for (int i = 0; i < (sizeof(bitmap) / sizeof(int)); i++)
			bitmap[i] = (rand() & 0xff) * 0x10101;
		
		[[self openGLContext] makeCurrentContext];
		
		glDisable(GL_LIGHTING);
		glDisable(GL_DITHER);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		
		glEnable(GL_TEXTURE_2D);
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0, GL_RGBA, 64, 64,
					 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
	}
	
	[pixelFormat release];
	
	return self;
}

- (void)drawRect:(NSRect)rect
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLoadIdentity();
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f,  1.0f);
	glEnd();
	
	[[self openGLContext] flushBuffer];
}

@end
