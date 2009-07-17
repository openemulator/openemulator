
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

- (id)initWithFrame:(NSRect)rect
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
	
	self = [super initWithFrame:rect pixelFormat:pixelFormat];
	if(self)
	{
		[[self openGLContext] makeCurrentContext];
		[[self openGLContext] update];
		
		GLint param = 1;
		CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &param);
		
		[self initGL];
	}
	
	[pixelFormat release];
	
	return self;
}

- (void)dealloc
{
	[self deallocGL];
	[super dealloc];
}

- (void)drawRect:(NSRect)rect
{
	//	printf("x:%f y:%f\n", rect.origin.x, rect.origin.y);
	//	printf("w:%f h:%f\n", rect.size.width, rect.size.height);
	
	NSRect viewRect = [self frame];
	float scale = [[self window] userSpaceScaleFactor];
	viewRect.size.width *= scale;
	viewRect.size.height *= scale;
	
	[self drawGL:viewRect];
	
	[[self openGLContext] flushBuffer];
}

- (void)loadGLTextureFromResource:(NSString *)name ofType:(NSString *)type toIndex:(int)index
{
	textureRect[index] = NSMakeRect(0, 0, 0, 0);
	NSString *path = [[NSBundle mainBundle] pathForResource:name ofType:type];
	NSBitmapImageRep *bitmap = [NSBitmapImageRep imageRepWithContentsOfFile:path];
	
	if (!bitmap)
		return;
	
	NSRect rect = NSMakeRect(0, 0, [bitmap size].width, [bitmap size].height);
	GLenum format = [bitmap hasAlpha] ? GL_RGBA : GL_RGB;
		
	textureRect[index] = rect;
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[index]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, format, rect.size.width, rect.size.height,
				 0, format, GL_UNSIGNED_BYTE, [bitmap bitmapData]);
}

- (void)renderGLTexture:(int)index toRect:(NSRect)rect withAlpha:(float)alpha
{
	glColor4f(1.0f, 1.0f, 1.0f, alpha);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[index]);
	
	NSRect texRect = textureRect[index];
	glBegin(GL_QUADS);
	glTexCoord2f(NSMinX(texRect), NSMinY(texRect));
	glVertex2f(NSMinX(rect), NSMaxY(rect));
	glTexCoord2f(NSMaxX(texRect), NSMinY(texRect));
	glVertex2f(NSMaxX(rect), NSMaxY(rect));
	glTexCoord2f(NSMaxX(texRect), NSMaxY(texRect));
	glVertex2f(NSMaxX(rect), NSMinY(rect));
	glTexCoord2f(NSMinX(texRect), NSMaxY(texRect));
	glVertex2f(NSMinX(rect), NSMinY(rect));
	glEnd();
}

- (void)initGL
{
	BOOL isPal = NO;
	float overscan = 0.138f;//0.08f a normal tv
	NSRect overscanRect;
	NSRect screenRect;
	NSRect renderRect;
	
	if (isPal)
	{
		overscanRect.size.width = 702;
		overscanRect.size.height = 576;
		renderRect.size.width = NSWidth(overscanRect) * (63.5556f/(14*64+16)*(40*14)/52.6556f);
	}
	else
	{
		overscanRect.size.width = 704;
		overscanRect.size.height = 483;
		renderRect.size.width = NSWidth(overscanRect) * (64.0f/(14*64+16)*(40*14)/52.0f);
	}
	
	overscanRect.origin.x = 0;
	overscanRect.origin.y = 0;
	
	screenRect = overscanRect;
	screenRect.size.width *= (1.0f - overscan);
	screenRect.size.height *= (1.0f - overscan);
	screenRect.origin.x = NSWidth(overscanRect) * overscan / 2.0f;
	screenRect.origin.y = NSHeight(overscanRect) * overscan / 2.0f;
	
	screenRectProportion = 4.0f / 3.0f;
	
	renderRect.size.height = 384.0f;
	renderRect.origin.x = (NSWidth(overscanRect) - NSWidth(renderRect)) / 2.0f;
	renderRect.origin.y = (NSHeight(overscanRect) - NSHeight(renderRect)) / 2.0f;
	
	viewWidthScale = NSWidth(renderRect) / NSWidth(screenRect);
	viewHeightScale = NSHeight(renderRect) / NSHeight(screenRect);
	viewOriginXScale = (NSMinX(renderRect) - NSMinX(screenRect)) / NSWidth(screenRect);
	viewOriginYScale = (NSMinY(renderRect) - NSMinY(screenRect)) / NSHeight(screenRect);
	
	cachedViewRect.size.width = 0;
	cachedViewRect.size.height = 0;
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glGenTextures((sizeof(texture) / sizeof(texture[0])), texture);
	
	[self loadGLTextureFromResource:@"OSDPower" ofType:@"png" toIndex:DV_TEXTURE_POWER];
	[self loadGLTextureFromResource:@"OSDPause" ofType:@"png" toIndex:DV_TEXTURE_PAUSE];
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[DV_TEXTURE_VIDEO]);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	[self renderGL];
}

- (void)deallocGL
{
	glDeleteTextures((sizeof(texture) / sizeof(texture[0])), texture);
}

- (void)renderGL
{
	int bitmap[560 * 384];
	
	for (int y = 0; y < 384; y++)
		for (int x = 0; x < 560; x++)
			bitmap[y * 560 + x] = (((x >> 1) & 0x1) ^ ((y >> 1) & 0x1)) * 0xffffff;
//			bitmap[y * 560 + x] = 0xffffff;

	textureRect[0] = NSMakeRect(0, 0, 560, 384);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[DV_TEXTURE_VIDEO]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGB, NSMaxX(textureRect[0]), NSMaxY(textureRect[0]),
				 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
}

- (void)drawGL:(NSRect)viewRect
{
	BOOL viewportUpdate = ((viewRect.origin.x != cachedViewRect.origin.x) ||
						   (viewRect.origin.y != cachedViewRect.origin.y) ||
						   (viewRect.size.width != cachedViewRect.size.width) ||
						   (viewRect.size.height != cachedViewRect.size.height));
	
	if (viewportUpdate | lastViewportUpdate);
	{
		lastViewportUpdate = viewportUpdate;
		cachedViewRect = viewRect;

		osdRect.size.width = textureRect[DV_TEXTURE_PAUSE].size.width / viewRect.size.width * 2.0;
		osdRect.size.height = textureRect[DV_TEXTURE_PAUSE].size.height / viewRect.size.height * 2.0;
		osdRect.origin.x = -osdRect.size.width / 2.0f;
		osdRect.origin.y = -osdRect.size.height / 2.0f;
		/*
		float viewRectProportion = NSWidth(viewRect) / NSHeight(viewRect);
		if (viewRectProportion > screenRectProportion)
		{
			float viewRectWidth = NSHeight(viewRect) * screenRectProportion;
			viewRect.origin.x += (NSWidth(viewRect) - viewRectWidth) / 2.0f;
			viewRect.size.width = viewRectWidth;
		}
		else
		{
			float viewRectHeight = NSWidth(viewRect) / screenRectProportion;
			viewRect.origin.y += (NSHeight(viewRect) - viewRectHeight) / 2.0f;
			viewRect.size.height = viewRectHeight;
		}
		
		viewRect.origin.x += NSWidth(viewRect) * viewOriginXScale;
		viewRect.origin.y += NSHeight(viewRect) * viewOriginYScale;
		viewRect.size.width *= viewWidthScale;
		viewRect.size.height *= viewHeightScale;
		*/
		glViewport(viewRect.origin.x, viewRect.origin.y,
				   viewRect.size.width, viewRect.size.height);
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		videoRect = NSMakeRect(-1, -1, 2, 2);
	}
	
//	[self renderGLTexture:DV_TEXTURE_VIDEO toRect:videoRect withAlpha:0.5f];
	[self renderGLTexture:DV_TEXTURE_POWER toRect:osdRect withAlpha:1.0f];
//	[self renderGLTexture:DV_TEXTURE_PAUSE toRect:osdRect withAlpha:1.0f];
}

@end
