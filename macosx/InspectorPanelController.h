
/**
 * OpenEmulator
 * Mac OS X Inspector Panel Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector.
 */

#import <Cocoa/Cocoa.h>

@interface InspectorPanelController : NSWindowController
{
    IBOutlet NSObjectController *fDocumentObjectController;
	
	IBOutlet NSMatrix *fTabMatrix;
    IBOutlet NSView *fEmulationView;
	IBOutlet NSView *fExpansionsView;
	IBOutlet NSView *fDiskDrivesView;
	IBOutlet NSView *fPeripheralsView;
	IBOutlet NSView *fVideoView;
	IBOutlet NSView *fAudioView;
	
    NSUserDefaults *fDefaults;
	id inspectedDocument;
	int oldTabTag;
}


- (void)activeDocumentDidChange;
- (id)inspectedDocument;
- (void)setInspectedDocument:(id)theDocument;

- (void)toggleInspectorPanel:(id)sender;

- (void)selectView:(id)sender;
- (void)setView:(int)tabTag isInit:(bool)isInit;

@end
