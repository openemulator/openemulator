
/**
 * OpenEmulator
 * Mac OS X Canvas Window Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas window.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

@interface CanvasWindowController : NSWindowController
{
	void *canvasComponent;
}

- (id)initWithCanvasComponent:(void *)theCanvasComponent;

@end
