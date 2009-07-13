
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
    NSUserDefaults *fDefaults;
    IBOutlet NSObjectController *documentController;
    
	id inspectedDocument;
	
	IBOutlet NSMatrix *fTabMatrix;
    IBOutlet NSView *fEmulationView, *fExpansionsView, *fDiskDrivesView,
		*fPortsView, *fVideoView, *fAudioView;
	int oldTabTag;
}

- (void)toggleInspectorPanel:(id)sender;

- (void)activeDocumentChanged;
- (id)inspectedDocument;
- (void)setInspectedDocument:(id)theDocument;

- (void)selectView:(id)sender;
- (void)setView:(int)tabTag isInit:(bool)isInit;

@end
