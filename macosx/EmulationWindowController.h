
/**
 * OpenEmulator
 * Mac OS X Emulation Window Controller
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation window.
 */

#import <Cocoa/Cocoa.h>

@class EmulationOutlineView;
@class EmulationItem;

@interface EmulationWindowController : NSWindowController
{
	IBOutlet id fSplitView;
	IBOutlet EmulationOutlineView *fOutlineView;
	
	IBOutlet id fDeviceBox;
	IBOutlet id fDeviceImage;
	IBOutlet id fDeviceState1Label;
	IBOutlet id fDeviceState1Value;
	IBOutlet id fDeviceState2Label;
	IBOutlet id fDeviceState2Value;
	IBOutlet id fDeviceState3Label;
	IBOutlet id fDeviceState3Value;
	IBOutlet id fDeviceState4Label;
	IBOutlet id fDeviceState4Value;
	IBOutlet id fDeviceButton;
	IBOutlet id fTableView;
	IBOutlet id fTableKeyColumn;
	IBOutlet id fTableValueColumn;
	
	EmulationItem *rootItem;
	
	EmulationItem *selectedItem;
	EmulationItem *clickedItem;
	
	NSButtonCell *checkBoxCell;
	NSPopUpButtonCell *popUpButtonCell;
	NSSliderCell *sliderCell;
}

- (void)updateSidebar;
- (void)updateWindowPosition;

- (EmulationItem *)itemForSender:(id)sender;
- (BOOL)mountImage:(NSString *)path inItem:(id)item;

- (IBAction)buttonAction:(id)sender;
- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)openStorage:(id)sender;
- (IBAction)unmountImage:(id)sender;
- (IBAction)deleteDevice:(id)sender;

@end
