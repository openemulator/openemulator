
/**
 * OpenEmulator
 * Mac OS X Preferences Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import <Cocoa/Cocoa.h>

@interface PreferencesController : NSWindowController {
	IBOutlet id fGeneralView;
}

- (void)selectView:(id)sender;
- (void)setView:(NSString *)itemIdentifier;

@end
