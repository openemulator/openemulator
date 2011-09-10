
/**
 * OpenEmulator
 * Mac OS X Canvas Toolbar View
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas toolbar view
 */

#import "CanvasToolbarView.h"

@implementation CanvasToolbarView

- (void)drawRect:(NSRect)rect
{
    NSImage *backgroundImage = [NSImage imageNamed:@"ToolbarBackground.png"];
    [backgroundImage drawInRect:[self bounds]
                       fromRect:NSZeroRect
                      operation:NSCompositeSourceOver
                       fraction:1.0f];
}

@end
