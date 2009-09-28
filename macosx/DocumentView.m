
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
	
	if(self = [super initWithFrame:rect pixelFormat:pixelFormat])
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

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{ 
    if ([keyPath isEqual:@"power"])
	{
		power = [[change objectForKey:NSKeyValueChangeNewKey] charValue];
		[self setNeedsDisplay:YES];
    }
	else if ([keyPath isEqual:@"pause"])
	{
		pause = [[change objectForKey:NSKeyValueChangeNewKey] charValue];
		[self setNeedsDisplay:YES];
	}
	else
		[super observeValueForKeyPath:keyPath
							 ofObject:object
							   change:change
							  context:context];
} 

- (void)drawRect:(NSRect)rect
{
	NSRect viewRect = [self frame];
	float scale = [[self window] userSpaceScaleFactor];
	viewRect.size.width *= scale;
	viewRect.size.height *= scale;
	
	[self drawGL:viewRect];
	
	[[self openGLContext] flushBuffer];
}

- (void)loadGLTextureFromResource:(NSString *)name
						   ofType:(NSString *)type
						  toIndex:(int)index
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
	float overscan = 0.137f;//0.08f a normal tv
	NSRect overscanRect;
	NSRect screenRect;
	NSRect renderRect;
	
	if (isPal)
	{
		overscanRect.size.width = 702;
		overscanRect.size.height = 576;
		renderRect.size.width = NSWidth(overscanRect) * (63.5556f/(14*64+16)*(40*14)/52.6556f);
		renderRect.size.height = 384.0f;
	}
	else
	{
		overscanRect.size.width = 704;
		overscanRect.size.height = 483;
		renderRect.size.width = NSWidth(overscanRect) * (64.0f/(14*64+16)*(40*14)/52.0f);
		renderRect.size.height = 384.0f;
	}
	
	overscanRect.origin.x = 0;
	overscanRect.origin.y = 0;
	
	screenRect = overscanRect;
	screenRect.size.width *= (1.0f - overscan);
	screenRect.size.height *= (1.0f - overscan);
	screenRect.origin.x = NSWidth(overscanRect) * overscan / 2.0f;
	screenRect.origin.y = NSHeight(overscanRect) * overscan / 2.0f;
	
	screenRectProportion = 4.0f / 3.0f;
	
	renderRect.origin.x = (NSWidth(overscanRect) - NSWidth(renderRect)) / 2.0f;
	renderRect.origin.y = (NSHeight(overscanRect) - NSHeight(renderRect)) / 2.0f;
	
	videoWidthScale = NSWidth(renderRect) / NSWidth(screenRect);
	videoHeightScale = NSHeight(renderRect) / NSHeight(screenRect);
	
	lastViewRect.size.width = 0;
	lastViewRect.size.height = 0;
	
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
	
	textureRect[0] = NSMakeRect(0, 0, 560, 384);
	
	for (int y = 0; y < textureRect[0].size.height; y++)
		for (int x = 0; x < textureRect[0].size.width; x++)
		{
			float l = (((x >> 0) & 0x1) ^ ((y >> 1) & 0x1)) * 0.5f + 0.5f;
			int r = l * 0x33;
			int g = l * 0xcc;
			int b = l * 0x44;
			bitmap[y * 560 + x] = (b << 16) | (g << 8) | r;
		}
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture[DV_TEXTURE_VIDEO]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGB, NSMaxX(textureRect[0]), NSMaxY(textureRect[0]),
				 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
}

- (void)drawGL:(NSRect)viewRect
{
	BOOL viewportUpdate = ((viewRect.origin.x != lastViewRect.origin.x) ||
						   (viewRect.origin.y != lastViewRect.origin.y) ||
						   (viewRect.size.width != lastViewRect.size.width) ||
						   (viewRect.size.height != lastViewRect.size.height));
	
	if (viewportUpdate);
	{
		lastViewRect = viewRect;
		
		float factor = NSWidth(viewRect) / NSHeight(viewRect) / screenRectProportion;
		if (factor > 1.0f)
		{
			videoRect.size.width = 2.0f / factor;
			videoRect.size.height = 2.0f;
		}
		else
		{
			videoRect.size.width = 2.0f;
			videoRect.size.height = 2.0f * factor;
		}
		
		videoRect.size.width *= videoWidthScale;
		videoRect.size.height *= videoHeightScale;
		videoRect.origin.x = -NSWidth(videoRect) / 2.0f;
		videoRect.origin.y = -NSHeight(videoRect) / 2.0f;
		
		osdRect.size.width = textureRect[DV_TEXTURE_PAUSE].size.width / viewRect.size.width * 2.0;
		osdRect.size.height = textureRect[DV_TEXTURE_PAUSE].size.height / viewRect.size.height * 2.0;
		osdRect.origin.x = -osdRect.size.width / 2.0f;
		osdRect.origin.y = -osdRect.size.height / 2.0f;
		
		glViewport(viewRect.origin.x, viewRect.origin.y,
				   viewRect.size.width, viewRect.size.height);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	if (power)
	{
		[self renderGLTexture:DV_TEXTURE_VIDEO toRect:videoRect withAlpha:(pause ? 0.5f : 1.0f)];
		if (pause)
			[self renderGLTexture:DV_TEXTURE_PAUSE toRect:osdRect withAlpha:1.0f];
	}
	else
		[self renderGLTexture:DV_TEXTURE_POWER toRect:osdRect withAlpha:1.0f];
}

@end
