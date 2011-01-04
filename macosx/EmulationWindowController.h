
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
	IBOutlet id fRemoveDevice;
	IBOutlet id fDeviceBox;
	IBOutlet id fDeviceImage;
	IBOutlet id fDeviceLine1Key;
	IBOutlet id fDeviceLine1Value;
	IBOutlet id fDeviceLine2Key;
	IBOutlet id fDeviceLine2Value;
	IBOutlet id fDeviceLine3Key;
	IBOutlet id fDeviceLine3Value;
	IBOutlet id fDeviceLine4Key;
	IBOutlet id fDeviceLine4Value;
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

- (void)updateWindowPosition;

- (EmulationItem *)itemForSender:(id)sender;
- (BOOL)mountImage:(NSString *)path inItem:(id)item;

- (IBAction)buttonAction:(id)sender;
- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)openStorage:(id)sender;
- (IBAction)unmountImage:(id)sender;

@end
