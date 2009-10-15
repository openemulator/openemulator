
/**
 * OpenEmulator
 * Mac OS X Preferences Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences.
 */

#import <Cocoa/Cocoa.h>

#import "TemplateChooser.h"

@interface PreferencesController : NSWindowController
{
	IBOutlet id fGeneralView;
	IBOutlet id fSoundView;
	
	IBOutlet id fShowTemplateChooserRadio;
	IBOutlet id fUseTemplateRadio;
	IBOutlet id fChooseTemplateButton;
	
	IBOutlet id fTemplateChooserSheet;
	IBOutlet id fTemplateChooserOutlineView;
	IBOutlet id fTemplateChooserChooserView;
	
	TemplateChooser *templateChooser;
}

- (void) setView:(NSString *) itemIdentifier;

- (void) updateUseDefaultTemplate;
- (void) setUseDefaultTemplate:(BOOL) useDefaultTemplate;
- (IBAction) selectTemplate:(id) sender;
- (IBAction) chooseTemplate:(id) sender;

- (IBAction) closeTemplateSheet:(id) sender;
- (void) templateChooserWasDoubleClicked:(id) sender;

@end
