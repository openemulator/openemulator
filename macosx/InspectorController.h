
/**
 * OpenEmulator
 * Mac OS X Inspector Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import <Cocoa/Cocoa.h>

#import "DeviceChooserController.h"
#import "Document.h"

@interface InspectorController : NSWindowController
{
	IBOutlet NSObjectController *fDocumentObjectController;
	
	IBOutlet NSMatrix *fTabMatrix;
	
	IBOutlet NSView *fEmulationView;
	IBOutlet NSView *fExpansionsView;
	IBOutlet NSView *fStorageView;
	IBOutlet NSView *fPeripheralsView;
	IBOutlet NSView *fAudioView;
	
	IBOutlet NSArrayController *fExpansionsController;
	IBOutlet NSArrayController *fStorageController;
	IBOutlet NSArrayController *fPeripheralsController;
	
	int selectedViewIndex;
	Document *inspectedDocument;
	
	IBOutlet id fDeviceChooserController;
}

- (void) activeDocumentDidChange;
- (id) inspectedDocument;
- (void) setInspectedDocument:(id) theDocument;

- (void) toggleInspector:(id) sender;

- (void) selectView:(id) sender;
- (void) setView:(int) viewIndex;

- (void) restoreWindowState:(id) sender;
- (void) storeWindowState:(id) sender;

- (void) addExpansion:(id) sender;
- (void) removeExpansion:(id) sender;
- (void) addStorage:(id) sender;
- (void) removeStorage:(id) sender;
- (void) addPeripheral:(id) sender;
- (void) removePeripheral:(id) sender;

- (void) removeDevice:(NSDictionary *) dict;

@end
