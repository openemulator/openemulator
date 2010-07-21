
/**
 * OpenEmulator
 * Mac OS X Inspector Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"
#import "DocumentController.h"
#import "DeviceChooserController.h"

@interface InspectorController : NSWindowController
{
	IBOutlet id fDocumentController;
	IBOutlet id fDeviceChooserController;
	
	IBOutlet NSObjectController *fDocumentObjectController;
	
	IBOutlet NSMatrix *fTabMatrix;
	
	IBOutlet NSView *fEmulationView;
	IBOutlet NSView *fExpansionsView;
	IBOutlet NSView *fStorageView;
	IBOutlet NSView *fPeripheralsView;
	IBOutlet NSView *fAudioView;
	
	int selectedViewIndex;
	
	Document *inspectedDocument;
	
	IBOutlet NSArrayController *fExpansionsController;
	IBOutlet NSArrayController *fStorageController;
	IBOutlet NSArrayController *fPeripheralsController;
	
	IBOutlet NSTextField *fPlayNameLabel;
	IBOutlet NSTextField *fPlayTimeLabel;
	IBOutlet NSTextField *fPlayDurationLabel;
	IBOutlet NSButton *fOpenPlayButton;
	IBOutlet NSButton *fTogglePlayButton;
	
	IBOutlet NSTextField *fRecordingTimeLabel;
	IBOutlet NSTextField *fRecordingSizeLabel;
	IBOutlet NSButton *fToggleRecordingButton;
	IBOutlet NSButton *fSaveRecordingAsButton;
}

- (void)activeDocumentDidChange;
- (id)inspectedDocument;
- (void)setInspectedDocument:(id)theDocument;

- (void)toggleInspector:(id)sender;

- (void)selectView:(id)sender;
- (void)setView:(int)viewIndex;

- (void)restoreWindowState:(id)sender;
- (void)storeWindowState:(id)sender;

- (void)addExpansion:(id)sender;
- (void)removeExpansion:(id)sender;
- (void)addStorage:(id)sender;
- (void)removeStorage:(id)sender;
- (void)addPeripheral:(id)sender;
- (void)removePeripheral:(id)sender;

- (void)removeDevice:(NSDictionary *)dict;

- (void)updatePlay;
- (IBAction)openPlay:(id)sender;
- (IBAction)togglePlay:(id)sender;

- (void)updateRecording;
- (IBAction)toggleRecording:(id)sender;
- (IBAction)saveRecording:(id)sender;

@end
