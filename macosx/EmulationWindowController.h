
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import <Cocoa/Cocoa.h>

#import "EmulationTableCell.h"

@interface EmulationWindowController : NSWindowController
{
	id fEmulationTableView;
	
	EmulationTableCell *emulationTableCell;
}

- (void)updateOptions;

- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)mountDevice:(id)sender;
- (IBAction)ejectDevice:(id)sender;

@end
