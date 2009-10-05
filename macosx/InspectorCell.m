
/**
 * OpenEmulator
 * Mac OS X Inspector Cell
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an inspector's cell.
 */

#import "InspectorCell.h"

@implementation InspectorCell

- (NSString *)toolTip
{
	return @"tool tip here";
}

/*- (NSAttributedString *)attributedStringValue
{
	NSMutableParagraphStyle * paragraphStyle;
	paragraphStyle = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	
	NSMutableAttributedString * fAttributedString;
	
	NSColor * titleColor, * statusColor;
    if ([self backgroundStyle] == NSBackgroundStyleDark)
        titleColor = statusColor = [NSColor whiteColor];
    else
    {
        titleColor = [NSColor controlTextColor];
        statusColor = [NSColor darkGrayColor];
    }
	
	NSDictionary *fTitleAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
									  [NSFont messageFontOfSize:12.0f],
									  NSFontAttributeName,
									  paragraphStyle,
									  NSParagraphStyleAttributeName,
									  titleColor,
									  NSForegroundColorAttributeName,
									  nil];
	fAttributedString = [[NSMutableAttributedString alloc] initWithString:@"Apple Disk II Interface Card\n"
															   attributes:fTitleAttributes];
	
	NSDictionary *fStatusAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
									   [NSFont messageFontOfSize:9.0f],
									   NSFontAttributeName,
									   paragraphStyle,
									   NSParagraphStyleAttributeName,
									   statusColor,
									   NSForegroundColorAttributeName,
									   nil];
	[fAttributedString appendAttributedString:[[NSMutableAttributedString alloc] initWithString:@"(on Apple IIGS Slot 6 Drive 1)"
											   "\nProDOS System Disk 2.0.3.dsk"
																					 attributes:fStatusAttributes]];
	
	return fAttributedString;
}*/

- (NSRect)titleRectForBounds:(NSRect)theRect
{
	NSRect titleFrame = [super titleRectForBounds:theRect];
	NSSize titleSize = [[self attributedStringValue] size];
	titleFrame.origin.y = theRect.origin.y + (theRect.size.height - titleSize.height) / 2.0;
	return titleFrame;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
    NSRect titleRect = [self titleRectForBounds:cellFrame];
    [[self attributedStringValue] drawInRect:titleRect];
}

@end
