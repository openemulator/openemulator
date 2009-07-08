
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
	IBOutlet id fGeneralView;
	IBOutlet id fSoundView;
}

- (void)selectView:(id)sender;
- (void)setView:(NSString *)itemIdentifier;
- (void)chooseTemplate:(id)sender;

@end
