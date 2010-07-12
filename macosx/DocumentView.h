
/**
 * OpenEmulator
 * Mac OS X Document View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation view.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#define DOCUMENT_KEYMAP_SIZE		256
#define DOCUMENT_MOUSE_BUTTONNUM	8

@interface DocumentView : NSOpenGLView
{
	CVDisplayLinkRef displayLink;
	void *oeglContext;

	int keyMap[DOCUMENT_KEYMAP_SIZE];
	int keyModifierFlags;
	int keyDownCount;
	
	BOOL mouseButtonState[DOCUMENT_MOUSE_BUTTONNUM];
	BOOL mouseCaptured;
	BOOL mouseCaptureRelease;
}

- (void) drawFrame;

@end
