
/**
 * OpenEmulator
 * Mac OS X Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>
#import "InspectorPanelController.h"

#define LINK_HOMEPAGE	"http://www.openemulator.org"
#define LINK_FORUMSURL	"http://groups.google.com/group/openemulator"
#define LINK_DEVURL	"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	"http://www.openemulator.org/donate.html"

@interface Controller : NSObject
{
	NSUserDefaults *fDefaults;
	NSWorkspace *fWorkspace;
	NSSpeechSynthesizer *fSpeechSynthesizer;
	
	IBOutlet InspectorPanelController *fInspectorPanelController;
}

- (void)linkHomepage:(id) sender;
- (void)linkForums:(id) sender;
- (void)linkDevelopment:(id) sender;
- (void)linkDonate:(id) sender;

@end
