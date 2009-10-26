
/**
 * OpenEmulator
 * Mac OS X Preferences Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences.
 */

#import <Cocoa/Cocoa.h>

#import "TemplateChooserViewController.h"

@interface PreferencesController : NSWindowController
{
	IBOutlet id fGeneralView;
	IBOutlet id fSoundView;
	
	IBOutlet id fShowTemplateChooserRadio;
	IBOutlet id fUseTemplateRadio;
	IBOutlet id fChooseTemplateButton;
	
	IBOutlet id fTemplateChooserSheet;
	IBOutlet id fTemplateChooserView;
	TemplateChooserViewController *templateChooserViewController;
}

- (void) setView:(NSString *) itemIdentifier;

- (void) updateUseDefaultTemplate;
- (void) setUseDefaultTemplate:(BOOL) useDefaultTemplate;
- (IBAction) selectUseDefaultTemplate:(id) sender;
- (IBAction) chooseDefaultTemplate:(id) sender;

- (IBAction) chooseTemplateInSheet:(id) sender;
- (IBAction) closeTemplateSheet:(id) sender;

@end
