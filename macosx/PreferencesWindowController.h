
/**
 * OpenEmulator
 * Mac OS X Preferences Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences window.
 */

#import <Cocoa/Cocoa.h>

#import "TemplateChooserViewController.h"

@interface PreferencesWindowController : NSWindowController
{
	IBOutlet id fGeneralView;
	IBOutlet id fAudioView;
	IBOutlet id fVideoView;
	
	IBOutlet id fShowTemplateChooserRadio;
	IBOutlet id fUseTemplateRadio;
	IBOutlet id fChooseTemplateButton;
	
	IBOutlet id fTemplateChooserSheet;
	IBOutlet id fTemplateChooserView;
	IBOutlet id fTemplateChooserChooseButton;
	TemplateChooserViewController *templateChooserViewController;
}

- (void)setView:(NSString *)itemIdentifier;

- (void)updateUseDefaultTemplate;
- (void)setUseDefaultTemplate:(BOOL)useDefaultTemplate;
- (IBAction)selectUseDefaultTemplate:(id)sender;
- (IBAction)chooseDefaultTemplate:(id)sender;

- (IBAction)chooseTemplateInSheet:(id)sender;
- (IBAction)closeTemplateSheet:(id)sender;

@end