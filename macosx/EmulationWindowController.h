
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import <Cocoa/Cocoa.h>

#import "EmulationItem.h"
#import "EmulationOutlineView.h"

@interface EmulationWindowController : NSWindowController
{
	id fSplitView;
	IBOutlet EmulationOutlineView *fOutlineView;
	id fRemoveDevice;
	id fDeviceBox;
	id fDeviceImage;
	id fDeviceLocationLabel;
	id fDeviceStateLabel;
	id fDeviceButton;
	id fTableView;
	id fTableKeyColumn;
	id fTableValueColumn;
	
	EmulationItem *rootItem;
	EmulationItem *selectedItem;
	
	NSButtonCell *checkBoxCell;
	NSPopUpButtonCell *popUpButtonCell;
	NSSliderCell *sliderCell;
}

- (void)updateWindowPosition;

- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)mountDevice:(id)sender;
- (IBAction)ejectDevice:(id)sender;

@end
