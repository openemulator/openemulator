
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
<NSToolbarDelegate, NSWindowDelegate>
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
- (void *)device;
- (void)closeWindow;
- (CanvasView *)canvasView;

- (IBAction)setHalfSize:(id)sender;
- (IBAction)setActualSize:(id)sender;
- (IBAction)setDoubleSize:(id)sender;
- (IBAction)fitToScreen:(id)sender;

@end
