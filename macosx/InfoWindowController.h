
/**
 * OpenEmulator
 * Mac OS X InfoWindow Controller
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>

@interface InfoWindowController : NSWindowController
{
	IBOutlet NSMatrix * fTabMatrix;
	int fCurrentTabTag;
	
    IBOutlet NSView * fInfoView, * fDiskDrivesView, * fExpansionView,
	* fVideoView, * fAudioView;
}

- (void) updateInfoStats;
- (void) updateInfoGeneral;
- (void) setTab:(id) sender;
- (NSView *) tabViewForTag:(int) tag;

@end
