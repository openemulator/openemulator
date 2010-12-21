
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

- (void)setDevicesInfo:(void *)theDevicesInfo
{
	devicesInfo = theDevicesInfo;
}

- (NSImage *)getImage:(NSString *)path
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	path = [resourcePath stringByAppendingPathComponent:path];
	
	return [[[NSImage alloc] initByReferencingFile:path] autorelease];
}

- (void)appendString:(NSString *)theString
  toAttributedString:(NSMutableAttributedString *)attrString
	  withAttributes:(NSDictionary *)attributes
{
	NSAttributedString *tempAttrString;
	tempAttrString = [[[NSAttributedString alloc] initWithString:theString
													  attributes:attributes]
					  autorelease];
	
	[attrString appendAttributedString:tempAttrString];
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
	[image setFlipped:YES];
	[image drawInRect:rect
			 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
			operation:NSCompositeSourceOver
			 fraction:1.0];
	
	[[NSGraphicsContext currentContext] setImageInterpolation:interpolation];
	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	// Set configuration
	float iconBorder = 3.0;
	float textBorder = 3.0;
	float buttonSize = 18.0;
	
	// Load data
	NSImage *icon = nil;
	NSString *titleString = [self stringValue];
	NSString *locationString = @"";
	NSString *statusString = @"";
	BOOL showable = NO;
	BOOL ejectable = NO;
	
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
				icon = [self getImage:getNSString(i->image)];
				titleString = getNSString(i->label);
				locationString = getNSString(i->location);
				statusString = getNSString(i->status);
				showable = (i->canvases.size() != 0);
				ejectable = i->mounted;
				
				break;
			}
		}
	}
	
	// Prepare attributes
	NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle
												 defaultParagraphStyle] mutableCopy]
											   autorelease];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	BOOL highlight = ([self backgroundStyle] == NSBackgroundStyleDark);
	NSColor *titleColor = (highlight ? [NSColor whiteColor] : [NSColor blackColor]);
	NSColor *statusColor = (highlight ? [NSColor whiteColor] : [NSColor darkGrayColor]);
	
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
	
	NSImage *showImage = [NSImage imageNamed:(highlight ? @"EmulationRevealSelected.png" :
											  @"EmulationReveal.png")];
	NSImage *ejectImage = [NSImage imageNamed:(highlight ? @"EmulationEjectSelected.png" :
											   @"EmulationEject.png")];
	
	// Set rectangles
	int buttonY = (cellFrame.size.height - buttonSize) / 2.0;
	
	float ejectWidth = ejectable ? buttonSize : 0.0;
	NSRect ejectRect = NSMakeRect((cellFrame.origin.x + cellFrame.size.width -
								   ejectWidth),
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
	NSRect attrStringRect = NSMakeRect(cellFrame.origin.x + cellFrame.size.height,
									   cellFrame.origin.y,
									   (showRect.origin.x - cellFrame.origin.x -
										cellFrame.size.height - textBorder),
									   cellFrame.size.height);
	
	// Prepare text
	NSMutableAttributedString *attrString;
	attrString = [[[NSMutableAttributedString alloc] init] autorelease];
	
	if ([locationString length])
	{
		[self appendString:[NSString stringWithFormat:@"%@", locationString]
		toAttributedString:attrString
			withAttributes:titleAttributes];
		[self appendString:[NSString localizedStringWithFormat:@" (%@)", titleString]
		toAttributedString:attrString
			withAttributes:statusAttributes];
	}
	else
		[self appendString:[NSString localizedStringWithFormat:@"%@", titleString]
		toAttributedString:attrString
			withAttributes:titleAttributes];
	
	if ([statusString length])
		[self appendString:[NSString localizedStringWithFormat:@"\n(%@)", statusString]
		toAttributedString:attrString
			withAttributes:statusAttributes];
	
	NSSize attrStringSize = [attrString size];
	attrStringRect.origin.y += (attrStringRect.size.height - attrStringSize.height) / 2.0;
	if (attrStringSize.width > attrStringRect.size.width)
		attrStringSize.width = attrStringRect.size.width;
	attrStringRect.size = attrStringSize;
	
	// Draw
	[self drawImage:icon inRect:iconRect];
	[attrString drawInRect:attrStringRect];
	[self drawImage:showImage inRect:showRect];
	[self drawImage:ejectImage inRect:ejectRect];
}

@end
