
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a window's content view (OpenGL).
 */

#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

#import "DocumentView.h"

@implementation DocumentView

- (id)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute pixelFormatAtrributes[] = {
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAWindow,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat;
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAtrributes];
	
	self = [super initWithFrame:frame pixelFormat:pixelFormat];
	if(self)
	{
		[[self openGLContext] makeCurrentContext];
		[[self openGLContext] update];
		
		GLint params[] =
		{
			1
		};
		CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, params);
		
		[self initGl];
	}
	
	[pixelFormat release];
	
	return self;
}

- (void)dealloc
{
	[self deallocGl];
	[super dealloc];
}

- (void)drawRect:(NSRect)rect
{
//	printf("x:%f y:%f\n", rect.origin.x, rect.origin.y);
//	printf("w:%f h:%f\n", rect.size.width, rect.size.height);
	[self drawGl:rect];
	
	[[self openGLContext] flushBuffer];
}



- (void)initGl
{
	BOOL isPal = NO;
	float overscan = 0.138f;//0.08f a normal tv
	NSRect overscanFrame;
	NSRect screenFrame;
	NSRect renderFrame;
	
	if (isPal)
	{
		overscanFrame.size.width = 702;
		overscanFrame.size.height = 576;
		renderFrame.size.width = NSWidth(overscanFrame) * (63.5556f/(14*64+16)*(40*14)/52.6556f);
	}
	else
	{
		overscanFrame.size.width = 704;
		overscanFrame.size.height = 483;
		renderFrame.size.width = NSWidth(overscanFrame) * (64.0f/(14*64+16)*(40*14)/52.0f);
	}
	
	overscanFrame.origin.x = 0;
	overscanFrame.origin.y = 0;
	
	screenFrame = overscanFrame;
	screenFrame.size.width *= (1.0f - overscan);
	screenFrame.size.height *= (1.0f - overscan);
	screenFrame.origin.x = NSWidth(overscanFrame) * overscan / 2.0f;
	screenFrame.origin.y = NSHeight(overscanFrame) * overscan / 2.0f;
	
	screenFrameProportion = 4.0f / 3.0f;
	
	renderFrame.size.height = 384.0f;
	renderFrame.origin.x = (NSWidth(overscanFrame) - NSWidth(renderFrame)) / 2.0f;
	renderFrame.origin.y = (NSHeight(overscanFrame) - NSHeight(renderFrame)) / 2.0f;
	
	viewWidthScale = NSWidth(renderFrame) / NSWidth(screenFrame);
	viewHeightScale = NSHeight(renderFrame) / NSHeight(screenFrame);
	viewOriginXScale = (NSMinX(renderFrame) - NSMinX(screenFrame)) / NSWidth(screenFrame);
	viewOriginYScale = (NSMinY(renderFrame) - NSMinY(screenFrame)) / NSHeight(screenFrame);
	
	bufferFrame.origin.x = 0;
	bufferFrame.origin.y = 0;
	bufferFrame.size.width = 561;
	bufferFrame.size.height = 384;
	
	cachedViewFrame.size.width = 0;
	cachedViewFrame.size.height = 0;
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	[self renderGl];
}

- (void)deallocGl
{
	glDeleteTextures(1, &textureId);
}

- (void)renderGl
{
	int bitmap[561 * 384];
	
	for (int y = 0; y < 384; y++)
		for (int x = 0; x < 560; x++)
			bitmap[y * 561 + x] = (((x >> 0) & 0x1) ^ ((y >> 1) & 0x1)) * 0xffffff;
	
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGBA, NSMaxX(bufferFrame), NSMaxY(bufferFrame),
				 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
}

- (void)drawGl:(NSRect)viewFrame
{
/*	if ((viewFrame.origin.x != cachedViewFrame.origin.x) ||
		(viewFrame.origin.y != cachedViewFrame.origin.y) ||
		(viewFrame.size.width != cachedViewFrame.size.width) ||
		(viewFrame.size.height != cachedViewFrame.size.height))
	{
*/		cachedViewFrame = viewFrame;
		
		float viewFrameProportion = NSWidth(viewFrame) / NSHeight(viewFrame);
		if (viewFrameProportion > screenFrameProportion)
		{
			float viewFrameWidth = NSHeight(viewFrame) * screenFrameProportion;
			viewFrame.origin.x += (NSWidth(viewFrame) - viewFrameWidth) / 2.0f;
			viewFrame.size.width = viewFrameWidth;
		}
		else
		{
			float viewFrameHeight = NSWidth(viewFrame) / screenFrameProportion;
			viewFrame.origin.y += (NSHeight(viewFrame) - viewFrameHeight) / 2.0f;
			viewFrame.size.height = viewFrameHeight;
		}
		
		viewFrame.origin.x += NSWidth(viewFrame) * viewOriginXScale;
		viewFrame.origin.y += NSHeight(viewFrame) * viewOriginYScale;
		viewFrame.size.width *= viewWidthScale;
		viewFrame.size.height *= viewHeightScale;
		
		glViewport(viewFrame.origin.x, viewFrame.origin.y,
				   viewFrame.size.width, viewFrame.size.height);
		
		glClear(GL_COLOR_BUFFER_BIT);
//	}
	
	glBegin(GL_QUADS);
	glTexCoord2f(NSMinX(bufferFrame), NSMinY(bufferFrame));
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(NSMaxX(bufferFrame), NSMinY(bufferFrame));
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(NSMaxX(bufferFrame), NSMaxY(bufferFrame));
	glVertex2f(1.0f,  -1.0f);
	glTexCoord2f(NSMinX(bufferFrame), NSMaxY(bufferFrame));
	glVertex2f(-1.0f, -1.0f);
	glEnd();
}

@end
