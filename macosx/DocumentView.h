
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a window's content view (OpenGL).
 */

#import <Cocoa/Cocoa.h>

@interface DocumentView : NSOpenGLView {
	GLuint textureId;
	
	float screenFrameProportion;
	float viewWidthScale;
	float viewHeightScale;
	float viewOriginXScale;
	float viewOriginYScale;
	NSRect bufferFrame;
	
	NSRect cachedViewFrame;
}

- (void)initGl;
- (void)deallocGl;
- (void)renderGl;
- (void)drawGl:(NSRect)viewFrame;

@end
