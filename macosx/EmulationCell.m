
/**
 * OpenEmulator
 * Mac OS X Emulation Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation cell.
 */

#import "EmulationCell.h"

@implementation EmulationCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
 	NSMutableAttributedString *aString;
	aString = [[NSMutableAttributedString alloc] initWithAttributedString:
			   [self attributedStringValue]];
	if (aString)
		[aString autorelease];
	
	if ([self backgroundStyle] == NSBackgroundStyleDark)
	{
		NSRange range;
		range.location = 0;
		range.length = [aString length];
		[aString removeAttribute:NSForegroundColorAttributeName range:range];
		[aString addAttribute:NSForegroundColorAttributeName
						value:[NSColor whiteColor]
						range:range];
	}
	
    NSSize contentSize = [self cellSize];
    cellFrame.origin.y += (cellFrame.size.height - contentSize.height) / 2.0;
    cellFrame.size.height = contentSize.height;
	
	[aString drawInRect:cellFrame];
}

@end
