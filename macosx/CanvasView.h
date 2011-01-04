
/**
 * OpenEmulator
 * Mac OS X Canvas View
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas view.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#define DEVICE_KEYMAP_SIZE		256
#define DEVICE_MOUSE_BUTTONNUM	8

@interface CanvasView : NSOpenGLView
{
	IBOutlet id fDocumentWindowController;
	
	void *openGLHAL;
	
	CVDisplayLinkRef displayLink;
	
	int keyMap[DEVICE_KEYMAP_SIZE];
	int keyModifierFlags;
}

- (void)drawFrame;

- (void)systemPowerDown:(id)sender;
- (void)systemSleep:(id)sender;
- (void)systemWakeUp:(id)sender;
- (void)systemColdRestart:(id)sender;
- (void)systemWarmRestart:(id)sender;
- (void)systemBreak:(id)sender;
- (void)systemDebuggerBreak:(id)sender;
- (void)applicationBreak:(id)sender;
- (void)applicationDebuggerBreak:(id)sender;
- (void)systemHibernate:(id)sender;

@end
