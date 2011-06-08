
/**
 * OpenEmulator
 * Mac OS X Library Table Cell
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a library table cell.
 */

#import "LibraryTableCell.h"
#import "LibraryItem.h"

#define IMAGE_PADDING 4
#define IMAGE_SIZE 48

@implementation LibraryTableCell : NSTextFieldCell

- (id)init
{
    self = [super init];
    
    if (self)
    {
        [self setLineBreakMode:NSLineBreakByTruncatingTail];
        [self setFont:[NSFont boldSystemFontOfSize:11]];
    }
    
    return self;
}

- (NSRect)imageRectForBounds:(NSRect)bounds
{
	LibraryItem *item = [self representedObject];
	if (![item image])
		return NSZeroRect;
	
	NSRect rect = bounds;
	NSRect paddingRect;
	NSDivideRect(rect, &paddingRect, &rect, IMAGE_PADDING, NSMinXEdge);
	NSDivideRect(rect, &rect, &paddingRect, IMAGE_SIZE, NSMinXEdge);
	return NSInsetRect(rect, 0, (NSHeight(bounds) - IMAGE_SIZE) / 2.0);
}

- (NSRect)textRectForBounds:(NSRect)bounds
{
	NSRect rect = bounds;
	NSRect paddingRect;
	
	NSDivideRect(rect, &paddingRect, &rect,
                 IMAGE_PADDING + IMAGE_SIZE + IMAGE_PADDING, NSMinXEdge);
	NSDivideRect(rect, &paddingRect, &rect,
                 IMAGE_PADDING, NSMaxXEdge);
    
	return NSInsetRect(rect, 0, (NSHeight(bounds) - [self cellSize].height) / 2.0);
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

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	LibraryItem *item = [self representedObject];
	
    cellFrame = NSInsetRect(cellFrame, 1.5, 0.5);
    
    NSBezierPath* thePath = [NSBezierPath bezierPath];
    [thePath setLineWidth:1];
    if (![self isHighlighted])
    {
        [[NSColor colorWithCalibratedWhite:0.94 alpha:1.0] setFill];
        [[NSColor colorWithCalibratedWhite:0.94 alpha:1.0] setStroke];
    }
    else if ([self backgroundStyle] == NSBackgroundStyleDark)
    {
        [[NSColor colorWithCalibratedRed:0.8 green:0.866 blue:0.941 alpha:1] setFill];
        [[NSColor colorWithCalibratedRed:0.157 green:0.306 blue:0.758 alpha:1] setStroke];
    }
    else
    {
        [[NSColor colorWithCalibratedWhite:0.83 alpha:1.0] setFill];
        [[NSColor colorWithCalibratedWhite:0.30 alpha:1.0] setStroke];
    }
    
    [thePath appendBezierPathWithRoundedRect:cellFrame xRadius:8 yRadius:8];
    
    [thePath fill];
    [thePath stroke];
    
    [self drawImage:[item image]
             inRect:[self imageRectForBounds:cellFrame]
          isFlipped:[controlView isFlipped]];
    
    NSMutableAttributedString *string;
    string = [[NSMutableAttributedString alloc] initWithAttributedString:
              [self attributedStringValue]];
    [string addAttribute:NSForegroundColorAttributeName
                   value:[NSColor blackColor]
                   range:NSMakeRange(0, [[self attributedStringValue] length])];
	[string drawInRect:[self textRectForBounds:cellFrame]];
    [string release];
}

@end
