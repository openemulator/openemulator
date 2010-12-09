
/**
 * OpenEmulator
 * Mac OS X Emulation Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation cell.
 */

#import <Cocoa/Cocoa.h>

@interface EmulationCell : NSTextFieldCell
{
}

- (IBAction)showDevice:(id)sender;
- (IBAction)revealDeviceInFinder:(id)sender;
- (IBAction)mountDevice:(id)sender;
- (IBAction)ejectDevice:(id)sender;

@end
