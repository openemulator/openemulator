
/**
 * OpenEmulator
 * Mac OS X Emulation Outline Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline cell.
 */

#import "EmulationOutlineCell.h"
#import "StringConversion.h"

#import "OEEmulation.h"

@implementation EmulationOutlineCell

- (void)dealloc
{
    [image release];
    image = nil;
	[uid release];
	uid = nil;
	
    [super dealloc];
}

- copyWithZone:(NSZone *)zone
{
    EmulationOutlineCell *cell = (EmulationOutlineCell *)[super copyWithZone:zone];
	
	cell->uid = [uid retain];
    cell->image = [image retain];
	
    return cell;
}

- (void)setUid:(NSString *)theUid
		 image:(NSImage *)theImage
	  showable:(BOOL)isShowable
	 ejectable:(BOOL)isEjectable
{
	if (uid != theUid)
	{
		[uid release];
		uid = [theUid retain];
	}
	if (image != theImage)
	{
		[image release];
		image = [theImage retain];
	}
	showable = isShowable;
	ejectable = isEjectable;
}

- (void)drawImage:(NSImage *)theImage inRect:(NSRect)rect isFlipped:(BOOL)flipped
{
	[theImage setFlipped:flipped];
	[theImage drawInRect:rect
				fromRect:NSMakeRect(0, 0, [theImage size].width, [theImage size].height)
			   operation:NSCompositeSourceOver
				fraction:1.0];
}

- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	// Configure
	float imageSize = 16.0;
	float textPadding = 3.0;
	float buttonSize = 18.0;
	
	// Prepare attributes
	BOOL selected = ([self backgroundStyle] == NSBackgroundStyleLowered);
	
	// Set rectangles
	float cellHeight = [self cellSize].height;
	
	NSRect imageRect, paddingRect, textRect, showRect, ejectRect;
	textRect = cellFrame;
	if (image)
	{
		NSDivideRect(textRect, &imageRect, &textRect, imageSize, NSMinXEdge);
		NSDivideRect(textRect, &paddingRect, &textRect, textPadding, NSMinXEdge);
		imageRect = NSInsetRect(imageRect, 0, (NSHeight(cellFrame) - imageSize) / 2.0);
	}
	if (showable)
	{
		NSDivideRect(textRect, &showRect, &textRect, showable ? buttonSize : 0, NSMaxXEdge);
		showRect = NSInsetRect(showRect, 0, (NSHeight(cellFrame) - buttonSize) / 2.0);
	}
	if (ejectable)
	{
		NSDivideRect(textRect, &ejectRect, &textRect, ejectable ? buttonSize : 0, NSMaxXEdge);
		ejectRect = NSInsetRect(ejectRect, 0, (NSHeight(cellFrame) - buttonSize) / 2.0);
	}
	NSDivideRect(textRect, &paddingRect, &textRect, textPadding, NSMaxXEdge);
	textRect = NSInsetRect(textRect, 0, (NSHeight(cellFrame) - cellHeight) / 2.0);
	
	// Draw
	[self drawImage:image inRect:imageRect isFlipped:[controlView isFlipped]];
	[super drawWithFrame:textRect inView:controlView];
	if (showable)
	{
		NSImage *showImage = [NSImage imageNamed:(selected ?
												  @"EmulationRevealSelected.png" :
												  @"EmulationReveal.png")];
		[self drawImage:showImage inRect:showRect isFlipped:[controlView isFlipped]];
	}
	if (ejectable)
	{
		NSImage *ejectImage = [NSImage imageNamed:(selected ?
												   @"EmulationEjectSelected.png" :
												   @"EmulationEject.png")];
		[self drawImage:ejectImage inRect:ejectRect isFlipped:[controlView isFlipped]];
	}
}

@end
