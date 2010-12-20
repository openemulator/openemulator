
/**
 * OpenEmulator
 * Mac OS X Emulation Table Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation table cell.
 */

#import "EmulationTableCell.h"

@implementation EmulationTableCell

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	float border = 3.0;
	
	// Load data
	NSString *id = [self stringValue];
	
	NSImage *icon;
	icon = [[NSWorkspace sharedWorkspace]
			iconForFileType:NSFileTypeForHFSTypeCode(kGenericFolderIcon)];
	NSString *titleString = @"Disk Drive II";
//	NSString *subtitleString = @"(on Apple II Slot 6 Drive 1)";
//	NSString *statusString = @"/Volumes/Applications/Disk Images/My Bad/test.dmg";
	NSString *subtitleString = @"/Volumes/Applications/Disk Images/My Bad/test.dmg";
	NSString *statusString = @"";
	
	// Setup drawing rectangle
	NSRect drawRect = cellFrame;
	drawRect.origin.x += 0.0;
	drawRect.size.width -= border;
	
	// Draw icon
	[[NSGraphicsContext currentContext] saveGraphicsState];
	NSImageInterpolation interpolation = [[NSGraphicsContext currentContext]
										  imageInterpolation];
	[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
	
	NSRect iconRect = drawRect;
	if ([controlView isFlipped])
	{
		NSAffineTransform *xform = [NSAffineTransform transform];
		[xform translateXBy:0.0 yBy:cellFrame.size.height];
		[xform scaleXBy:1.0 yBy:-1.0];
		[xform concat];
		iconRect.origin.y = -iconRect.origin.y;
	}
	iconRect = NSMakeRect(iconRect.origin.x + border,
						  iconRect.origin.y + border,
						  iconRect.size.height - 2.0 * border,
						  iconRect.size.height - 2.0 * border);
	[icon drawInRect:iconRect
			fromRect:NSMakeRect(0, 0, [icon size].width, [icon size].height)
		   operation:NSCompositeSourceOver
			fraction:1.0];
	
	[[NSGraphicsContext currentContext] setImageInterpolation:interpolation];
	[[NSGraphicsContext currentContext] restoreGraphicsState];
	drawRect.origin.x += drawRect.size.height;
	drawRect.size.width -= drawRect.size.height;
	
	// Prepare text attributes
	NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle
												 defaultParagraphStyle] mutableCopy]
											   autorelease];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	NSColor *titleColor = ([self isHighlighted] ? [NSColor whiteColor] :
						   [NSColor blackColor]);
	NSColor *subtitleColor = ([self isHighlighted] ? [NSColor whiteColor] :
							  [NSColor darkGrayColor]);
	
	NSMutableDictionary *titleAttributes;
	titleAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
					   [NSFont messageFontOfSize:12.0], NSFontAttributeName,
					   titleColor, NSForegroundColorAttributeName,
					   paragraphStyle, NSParagraphStyleAttributeName,
					   nil];
	
	NSMutableDictionary *subtitleAttributes;
	subtitleAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
						  [NSFont messageFontOfSize:9.0], NSFontAttributeName,
						  subtitleColor, NSForegroundColorAttributeName,
						  paragraphStyle, NSParagraphStyleAttributeName,
						  nil];
	
	// Prepare text
	NSAttributedString *attrTitleString;
	attrTitleString = [[NSAttributedString alloc] initWithString:titleString
													  attributes:titleAttributes];
	[attrTitleString autorelease];
	NSSize titleSize = [attrTitleString size];
	NSAttributedString *attrSubtitleString;
	attrSubtitleString = [[NSAttributedString alloc] initWithString:subtitleString
														 attributes:subtitleAttributes];
	[attrSubtitleString autorelease];
	NSSize subtitleSize = [attrSubtitleString size];
	NSAttributedString *attrStatusString;
	attrStatusString = [[NSAttributedString alloc] initWithString:statusString
													   attributes:subtitleAttributes];
	[attrStatusString autorelease];
	NSSize statusSize = [attrStatusString size];
	
	if (titleSize.width > drawRect.size.width)
		titleSize.width = drawRect.size.width;
	if (subtitleSize.width > drawRect.size.width)
		subtitleSize.width = drawRect.size.width;
	if (statusSize.width > drawRect.size.width)
		statusSize.width = drawRect.size.width;
	
	if (statusSize.width == 0.0)
		statusSize.height = 0.0;
	float textHeight = titleSize.height + subtitleSize.height + statusSize.height;
	float textY = drawRect.origin.y + (drawRect.size.height - textHeight) / 2.0;
	
	NSRect titleRect = NSMakeRect(drawRect.origin.x, textY,
								  titleSize.width, titleSize.height);
	textY += titleSize.height;
	NSRect subtitleRect = NSMakeRect(drawRect.origin.x, textY,
									 subtitleSize.width, subtitleSize.height);
	textY += subtitleSize.height;
	NSRect statusRect = NSMakeRect(drawRect.origin.x, textY,
								   statusSize.width, statusSize.height);
	
	// Draw text
	[attrTitleString drawInRect:titleRect];
	[attrSubtitleString drawInRect:subtitleRect];
	[attrStatusString drawInRect:statusRect];
}

@end
