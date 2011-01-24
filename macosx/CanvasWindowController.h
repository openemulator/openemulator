
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
	void *canvas;
	IBOutlet CanvasView *fCanvasView;
	
	NSString *title;
}

- (id)initWithTitle:(NSString *)title
		  canvas:(void *)theCanvas;
- (void *)canvas;
- (void)destroyCanvas;

@end
