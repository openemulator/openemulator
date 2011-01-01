
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
	BOOL buttonRollover;
	BOOL buttonPressed;
}

- (void)setButtonRollover:(BOOL)rollover;

- (void)addTrackingAreasForView:(NSView *)controlView
						 inRect:(NSRect)cellFrame
				   withUserInfo:(NSDictionary *)userInfo
				  mouseLocation:(NSPoint)mouseLocation;

@end
