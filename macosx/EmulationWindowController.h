
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import <Cocoa/Cocoa.h>

#import "EmulationOutlineItem.h"
#import "EmulationOutlineCell.h"
#import "VerticallyCenteredTextFieldCell.h"

@interface EmulationWindowController : NSWindowController
{
	id fSplitView;
	id fOutlineView;
	id fTableView;
	id fTableKeyColumn;
	id fTableValueColumn;
	
	EmulationOutlineItem *contents;
	
	VerticallyCenteredTextFieldCell *textCell;
	NSButtonCell *checkBoxCell;
	NSPopUpButtonCell *popUpButtonCell;
	NSSliderCell *sliderCell;
	
	NSMutableArray *info;
}

- (void)updateWindowPosition;

- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)mountDevice:(id)sender;
- (IBAction)ejectDevice:(id)sender;

@end
