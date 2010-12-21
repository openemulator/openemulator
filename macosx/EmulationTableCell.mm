
/**
 * OpenEmulator
 * Mac OS X Emulation Table Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation table cell.
 */

#import "EmulationTableCell.h"
#import "StringConversion.h"

#import "OEEmulation.h"

@implementation EmulationTableCell

- (id)init
{
	self = [super init];
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
	NSLog(@"copyWithZone");
	
	return [super copyWithZone:zone];
}

- (void)drawImage:(NSImage *)image inRect:(NSRect)rect
{
	[[NSGraphicsContext currentContext] saveGraphicsState];
	NSImageInterpolation interpolation = [[NSGraphicsContext currentContext]
										  imageInterpolation];
	[[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
	
//	if ([controlView isFlipped])
//	{
//		NSAffineTransform *xform = [NSAffineTransform transform];
//		[xform translateXBy:0.0 yBy:cellFrame.size.height];
//		[xform scaleXBy:1.0 yBy:-1.0];
//		[xform concat];
//		rect.origin.y = -rect.origin.y;
//	}
	[image drawInRect:rect
			 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
			operation:NSCompositeSourceOver
			 fraction:1.0];
	
	[[NSGraphicsContext currentContext] setImageInterpolation:interpolation];
	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

- (void)setDevicesInfo:(void *)theDevicesInfo
{
	devicesInfo = theDevicesInfo;
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	// Set configuration
	float iconBorder = 3.0;
	float buttonSize = 18.0;
	
	// Load data
	NSImage *icon = nil;
	NSString *titleString = [self stringValue];
	NSString *statusString = @"";
	BOOL showable = NO;
	NSImage *showImage = nil;
	BOOL ejectable = NO;
	NSImage *ejectImage = nil;
	
	OEDevicesInfo *theDevicesInfo = (OEDevicesInfo *)devicesInfo;
	string id = getCPPString([self stringValue]);
	if (theDevicesInfo)
	{
		for (OEDevicesInfo::iterator i = theDevicesInfo->begin();
			 i != theDevicesInfo->end();
			 i++)
		{
			if (id == i->id)
			{
				titleString = getNSString(i->label);
				if (i->status != "")
				{
					if ([statusString length])
						statusString = [statusString stringByAppendingString:@" "];
					
					NSString *theStatusString;
					theStatusString = [NSString stringWithFormat:@"(%@)",
									getNSString(i->status)];
					
					statusString = [statusString stringByAppendingString:theStatusString];
				}
				if (i->location != "")
				{
					NSString *locationString;
					locationString = [NSString localizedStringWithFormat:@"on %@",
									  getNSString(i->location)];
					
					statusString = [statusString stringByAppendingString:locationString];
				}
 				
				showable = (i->canvases.size() != 0);
				ejectable = i->mounted;
				
				break;
			}
		}
	}
	
	// Prepare text attributes
	NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle
												 defaultParagraphStyle] mutableCopy]
											   autorelease];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	NSColor *titleColor = ([self isHighlighted] ? [NSColor whiteColor] :
						   [NSColor blackColor]);
	NSColor *statusColor = ([self isHighlighted] ? [NSColor whiteColor] :
							[NSColor darkGrayColor]);
	
	NSMutableDictionary *titleAttributes;
	titleAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
					   [NSFont messageFontOfSize:12.0], NSFontAttributeName,
					   titleColor, NSForegroundColorAttributeName,
					   paragraphStyle, NSParagraphStyleAttributeName,
					   nil];
	
	NSMutableDictionary *statusAttributes;
	statusAttributes = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
						[NSFont messageFontOfSize:9.0], NSFontAttributeName,
						statusColor, NSForegroundColorAttributeName,
						paragraphStyle, NSParagraphStyleAttributeName,
						nil];
	
	// Set rectangles
	int buttonY = (cellFrame.size.height - buttonSize) / 2.0;
	
	float ejectWidth = ejectable ? buttonSize : 0.0;
	NSRect ejectRect = NSMakeRect(cellFrame.origin.x + cellFrame.size.width - ejectWidth,
								  cellFrame.origin.y + buttonY,
								  ejectWidth,
								  buttonSize);
	float showWidth = showable ? buttonSize : 0.0;
	NSRect showRect = NSMakeRect(ejectRect.origin.x - showWidth,
								 cellFrame.origin.y + buttonY,
								 showWidth,
								 buttonSize);
	NSRect iconRect = NSMakeRect(cellFrame.origin.x + iconBorder,
								 cellFrame.origin.y + iconBorder,
								 cellFrame.size.height - 2.0 * iconBorder,
								 cellFrame.size.height - 2.0 * iconBorder);
	NSRect textRect = NSMakeRect(cellFrame.origin.x + cellFrame.size.height,
								 cellFrame.origin.y,
								 showRect.origin.x - cellFrame.origin.x - cellFrame.size.height,
								 cellFrame.size.height);
	
	// Prepare text
	NSAttributedString *attrTitleString;
	attrTitleString = [[NSAttributedString alloc] initWithString:titleString
													  attributes:titleAttributes];
	[attrTitleString autorelease];
	NSSize titleSize = [attrTitleString size];
	NSAttributedString *attrStatusString;
	attrStatusString = [[NSAttributedString alloc] initWithString:statusString
													   attributes:statusAttributes];
	[attrStatusString autorelease];
	NSSize statusSize = [attrStatusString size];
	
	if (titleSize.width > textRect.size.width)
		titleSize.width = textRect.size.width;
	if (statusSize.width > textRect.size.width)
		statusSize.width = textRect.size.width;
	
	if (statusSize.width == 0.0)
		statusSize.height = 0.0;
	
	float textHeight = titleSize.height + statusSize.height;
	float textY = textRect.origin.y + (textRect.size.height - textHeight) / 2.0;
	
	NSRect titleRect = NSMakeRect(textRect.origin.x,
								  textY,
								  titleSize.width,
								  titleSize.height);
	textY += titleSize.height;
	NSRect statusRect = NSMakeRect(textRect.origin.x,
								   textY,
								   statusSize.width,
								   statusSize.height);
	
	// Draw
	[self drawImage:icon inRect:iconRect];
	[attrTitleString drawInRect:titleRect];
	[attrStatusString drawInRect:statusRect];
	[self drawImage:showImage inRect:showRect];
	[self drawImage:ejectImage inRect:ejectRect];
}

@end
