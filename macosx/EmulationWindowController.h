
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
<NSToolbarDelegate, NSOutlineViewDelegate,
NSOutlineViewDataSource, NSComboBoxDataSource>
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
	
	NSButtonCell *checkBoxCell;
	NSPopUpButtonCell *popUpButtonCell;
	NSSliderCell *sliderCell;
}

- (void)updateEmulation:(id)sender;

- (EmulationItem *)itemForSender:(id)sender;
- (BOOL)selectItem:(EmulationItem *)item withUid:(NSString *)uid;

- (IBAction)buttonAction:(id)sender;
- (IBAction)showDevice:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)openDiskImage:(id)sender;
- (IBAction)unmount:(id)sender;
- (IBAction)delete:(id)sender;

@end
