
/**
 * OpenEmulator
 * Mac OS X Emulation Outline Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation outline cell.
 */

#import <Cocoa/Cocoa.h>

@interface EmulationOutlineCell : NSTextFieldCell
{
	NSString *uid;
	NSImage *image;
	BOOL showable;
	BOOL ejectable;
}

- (void)setUid:(NSString *)theUid
		 image:(NSImage *)theImage
	  showable:(BOOL)isShowable
	 ejectable:(BOOL)isEjectable;

@end
