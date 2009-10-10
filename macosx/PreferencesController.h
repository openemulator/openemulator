
/**
 * OpenEmulator
 * Mac OS X Preferences Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences.
 */

#import <Cocoa/Cocoa.h>

@interface PreferencesController : NSWindowController
{
	NSUserDefaults *userDefaults;
	
	IBOutlet id fGeneralView;
	IBOutlet id fSoundView;
	
	IBOutlet id fDisableDefaultTemplate;
	IBOutlet id fEnableDefaultTemplate;
	IBOutlet id fChooseTemplateButton;
	
	IBOutlet id fTemplateChooserSheet;
}

- (void) selectView:(id) sender;
- (void) setView:(NSString *) itemIdentifier;

- (IBAction) selectTemplate:(id) sender;
- (IBAction) chooseTemplate:(id) sender;
- (IBAction) useDefaultTemplate:(BOOL) useDefaultTemplate;
- (IBAction) closeTemplateSheet:(id) sender;
- (IBAction) chooseTemplateSheet:(id) sender;

@end
