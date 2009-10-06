
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

- (NSAttributedString *)attributedStringValue
{
	NSString *firstLine;
	NSString *nextLines;
	NSRange range = [[self stringValue] rangeOfString:@"\n"];
	if (range.location != NSNotFound)
	{
		firstLine = [[self stringValue] substringToIndex:range.location + 1];
		nextLines = [[self stringValue] substringFromIndex:range.location + 1];
	}
	else {
		firstLine = [self stringValue];
		nextLines = @"";
	}
	
	NSMutableParagraphStyle * paragraphStyle;
	paragraphStyle = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	
	NSColor * titleColor, * statusColor;
    if ([self backgroundStyle] == NSBackgroundStyleDark)
        titleColor = statusColor = [NSColor whiteColor];
    else
    {
        titleColor = [NSColor controlTextColor];
        statusColor = [NSColor darkGrayColor];
    }
	
	NSMutableAttributedString * fAttributedString;
	NSDictionary *fTitleAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
									  [NSFont messageFontOfSize:12.0f],
									  NSFontAttributeName,
									  paragraphStyle,
									  NSParagraphStyleAttributeName,
									  titleColor,
									  NSForegroundColorAttributeName,
									  nil];
	fAttributedString = [[NSMutableAttributedString alloc] initWithString:firstLine
															   attributes:fTitleAttributes];
	
	NSDictionary *fStatusAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
									   [NSFont messageFontOfSize:9.0f],
									   NSFontAttributeName,
									   paragraphStyle,
									   NSParagraphStyleAttributeName,
									   statusColor,
									   NSForegroundColorAttributeName,
									   nil];
	[fAttributedString appendAttributedString:[[NSMutableAttributedString alloc] initWithString:nextLines
																					 attributes:fStatusAttributes]];
	
	return fAttributedString;
}

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
