
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation view.
 */

#import <Cocoa/Cocoa.h>

#define DV_TEXTURE_VIDEO	0
#define DV_TEXTURE_POWER	1
#define DV_TEXTURE_PAUSE	2
#define DV_TEXTURE_END		3

@interface DocumentView : NSOpenGLView {
	GLuint texture[DV_TEXTURE_END];
	NSRect textureRect[DV_TEXTURE_END];
	
	float screenRectProportion;
	float videoWidthScale;
	float videoHeightScale;
	
	NSRect lastViewRect;
	
	NSRect videoRect;
	NSRect osdRect;
	
	BOOL power;
	BOOL pause;
}

- (void) initGL;
- (void) deallocGL;
- (void) renderGL;
- (void) drawGL:(NSRect)viewFrame;

@end
