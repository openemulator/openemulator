
/**
 * OpenEmulator
 * Mac OS X InspectorPanel Controller
 * (C) 2008-2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>

@interface InspectorPanelController : NSWindowController
{
    NSUserDefaults * fDefaults;
    IBOutlet NSObjectController *documentController;
    
	id inspectedDocument;
	
	IBOutlet NSMatrix * fTabMatrix;
    IBOutlet NSView * fEmulationView, * fExpansionsView, * fDiskDrivesView,
		* fPortsView, * fVideoView, * fAudioView;
	int oldTabTag;
}

- (void)toggleInspectorPanel:(id)sender;
- (void)activeDocumentChanged;
- (void)selectView:(id)sender;
- (void)setView:(int)tabTag isInit:(bool)isInit;

@end
