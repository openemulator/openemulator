
/**
 * OpenEmulator
 * Mac OS X Canvas Print View
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a canvas print view.
 */

#import "CanvasPrintView.h"

@implementation CanvasPrintView

- (id)initWithCanvasView:(CanvasView *)theCanvasView
{
	NSSize frameSize = [theCanvasView canvasSize];
	NSRect frame = NSMakeRect(0,
							  0,
							  frameSize.width,
							  frameSize.height);
	
	if (self = [super initWithFrame:frame])
	{
		canvasView = theCanvasView;
	}
	
	return self;
}

- (void)drawImage:(NSImage *)theImage inRect:(NSRect)rect isFlipped:(BOOL)flipped
{
	BOOL wasFlipped = [theImage isFlipped];
	
	[theImage setFlipped:flipped];
	[theImage drawInRect:rect
				fromRect:NSZeroRect
			   operation:NSCompositeSourceOver
				fraction:1.0];
	
	[theImage setFlipped:wasFlipped];
}

- (void)drawRect:(NSRect)theRect
{
	NSBitmapImageRep *rep;
	
	rep = [canvasView page:0];
	
	NSImage *image = [[[NSImage alloc] init] autorelease];
	[image addRepresentation:rep];
	
	[self drawImage:image
			 inRect:theRect
		  isFlipped:[self isFlipped]];
}

- (BOOL)knowsPageRange:(NSRangePointer)aRange
{
	NSRect bounds = [self bounds];
	NSSize pageSize = [canvasView pageSize];
	aRange->location = 1;
	aRange->length = NSHeight(bounds) / pageSize.height + 1;
	
	return YES;
}

- (NSRect)rectForPage:(NSInteger)pageNumber
{
	NSSize pageSize = [canvasView pageSize];
	
	return NSMakeRect(0,
					  pageNumber * pageSize.height,
					  pageSize.width,
					  pageSize.height);
}

@end
