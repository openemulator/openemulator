
/**
 * OpenEmulator
 * Mac OS X Canvas Window Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas window.
 */

#import <Cocoa/Cocoa.h>

#import "CanvasView.h"

@interface CanvasWindowController : NSWindowController
<NSToolbarDelegate>
{
	IBOutlet id fScrollView;
	IBOutlet CanvasView *fCanvasView;
	IBOutlet id fToolbarView;
	
	void *device;
	NSString *title;
	void *canvas;
}

- (id)initWithDevice:(void *)theDevice
			   title:(NSString *)theTitle
			  canvas:(void *)theCanvas;
- (void *)canvas;
- (void)closeWindow;
- (CanvasView *)canvasView;

- (void)systemPowerDown:(id)sender;
- (void)systemSleep:(id)sender;
- (void)systemWakeUp:(id)sender;
- (void)systemColdRestart:(id)sender;
- (void)systemWarmRestart:(id)sender;
- (void)systemDebuggerBreak:(id)sender;

@end
