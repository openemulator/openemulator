
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
		NSOpenGLPFADoubleBuffer, 1,
		NSOpenGLPFAWindow, 1,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		0
	};
	
	NSOpenGLPixelFormat *pixelFormat;
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAtrributes];
	
	self = [super initWithFrame:frame pixelFormat:pixelFormat];
	if(self)
	{
		[[self openGLContext] makeCurrentContext];
		
		[self initGl];
		
		NSTimer *timer;
		timer = [NSTimer scheduledTimerWithTimeInterval:1/60.0f
												 target:self
											   selector:@selector(tick:)
											   userInfo:NULL
												repeats:YES];	
	}
	
	[pixelFormat release];
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
	[self deallocGl];
}

- (void)tick:(NSTimer *)timer
{
	[[self openGLContext] makeCurrentContext];
	[self renderGl];
	
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect
{
	[self drawGl:rect];
	
	[[self openGLContext] flushBuffer];
}



- (void)initGl
{
	BOOL isPal = NO;
	float overscan = 0.08f;
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
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, textureId);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	[self renderGl];
}

- (void)deallocGl
{
	glDeleteTextures(1, &textureId);
}

- (void)renderGl
{
	int bitmap[561 * 384];
	
	for (int i = 0; i < (sizeof(bitmap) / sizeof(int)); i++)
		bitmap[i] = (random() & 0xff) * 0x010101;
	
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGBA, NSMaxX(bufferFrame), NSMaxY(bufferFrame),
				 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
}

- (void)drawGl:(NSRect)viewFrame
{
	if ((viewFrame.origin.x != cachedViewFrame.origin.x) ||
		(viewFrame.origin.y != cachedViewFrame.origin.y) ||
		(viewFrame.size.width != cachedViewFrame.size.width) ||
		(viewFrame.size.height != cachedViewFrame.size.height))
	{
		cachedViewFrame = viewFrame;
		
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
	}
	
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
