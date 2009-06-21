
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <OpenGL/gl.h>

#import "DocumentView.h"

@implementation DocumentView

- (void)initGl
{
	
    glDeleteTextures(1, &textureId);
	glGenTextures(1, &textureId);
}

- (void)renderGl
{
	int bitmap[560 * 192];
	
	for (int i = 0; i < (sizeof(bitmap) / sizeof(int)); i++)
		bitmap[i] = (random() & 0xff) * 0x10101;
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, textureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGBA, 560, 192,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
}

- (void)viewGl
{
	GLfloat texX0, texY0;
	GLfloat texX1, texY1;
	
/*    texX0 = (float)p_vout->fmt_out.i_x_offset;
    texY0 = (float)p_vout->fmt_out.i_y_offset;
    texX1 = texX0 + (float)p_vout->fmt_out.i_visible_width;
	texY1 = texY0 + (float)p_vout->fmt_out.i_visible_height;
*/
	texX0 = 0;
	texY0 = 0;
	texX1 = 560;
	texY1 = 192;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glBegin(GL_POLYGON);
	glTexCoord2f(texX0, texY0);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(texX1, texY0);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(texX1, texY1);
	glVertex2f(1.0f,  -1.0f);
	glTexCoord2f(texX0, texY1);
	glVertex2f(-1.0f, -1.0f);
	glEnd();
	
	glDisable(GL_TEXTURE_RECTANGLE_EXT);
}

- (void)tick:(NSTimer *)timer
{
	[[self openGLContext] makeCurrentContext];
	[self renderGl];
	
	[self setNeedsDisplay:YES];
}	

- (id)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute pixelFormatAtrributes[] = {
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAWindow,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAAccumSize, 0,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat;
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAtrributes];
	
	self = [super initWithFrame:frame pixelFormat:pixelFormat];
	if(self)
	{
		[[self openGLContext] makeCurrentContext];
		
		[self renderGl];
		
		NSTimer *timer;
		timer = [NSTimer scheduledTimerWithTimeInterval:0.01666
												 target:self
											   selector:@selector(tick:)
											   userInfo:NULL
												repeats:YES];	
	}
	
	[pixelFormat release];
	
	return self;
}

- (void)drawRect:(NSRect)rect
{
	NSRect viewRect = [self bounds];
	glViewport (0, 0, viewRect.size.width, viewRect.size.height);
	
	[self viewGl];
	
	[[self openGLContext] flushBuffer];
}

@end
