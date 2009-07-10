
/**
 * OpenEmulator
 * Mac OS X Preferences Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences.
 */

#import <Cocoa/Cocoa.h>

@interface PreferencesController : NSWindowController {
	NSUserDefaults *userDefaults;
	
	IBOutlet id fGeneralView;
	IBOutlet id fSoundView;
	
	IBOutlet id fShowTemplateChooserCell;
	IBOutlet id fUseTemplateCell;
	IBOutlet id fChooseTemplateButton;
	
	IBOutlet id fChooseTemplateSheet;
}

- (void)selectView:(id)sender;
- (void)setView:(NSString *)itemIdentifier;
- (void)selectTemplate:(id)sender;
- (void)chooseTemplate:(id)sender;
- (IBAction)closeTemplateSheet:(id)sender;

@end
