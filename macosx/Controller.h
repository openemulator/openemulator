
/**
 * OpenEmulator
 * Mac OS X Controller
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>

#define APPLEIIGO_HOMEPAGE	"http://www.openemulator.org"
#define APPLEIIGO_FORUMSURL	"http://groups.google.com/group/openemulator"
#define APPLEIIGO_DEVURL	"http://code.google.com/p/openemulator"
#define APPLEIIGO_DONATEURL	"http://www.openemulator.org/donate.html"

@interface Controller : NSObject
{
/*    NSUserDefaults * fDefaults;
    
	InfoWindowController * fInfoController;
	
	NSSpeechSynthesizer * speechSynthesizer;
	
	IBOutlet NSWindow * fWindow;*/
}

- (IBAction) linkHomepage:(id) sender;
- (IBAction) linkForums:(id) sender;
- (IBAction) linkDevelopment:(id) sender;
- (IBAction) linkDonate:(id) sender;

@end
