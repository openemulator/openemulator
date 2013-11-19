
/**
 * OpenEmulator
 * Mac OS X Canvas Print View
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas print view
 */

#import <Cocoa/Cocoa.h>

#import "CanvasView.h"

@interface CanvasPrintView : NSView
{
    CanvasView *canvasView;
}

- (id)initWithCanvasView:(CanvasView *)theCanvasView;

@end
