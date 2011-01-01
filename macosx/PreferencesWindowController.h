
/**
 * OpenEmulator
 * Mac OS X Preferences Window Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences window.
 */

#import <Cocoa/Cocoa.h>

#import "TemplateChooserViewController.h"

@interface PreferencesWindowController : NSWindowController
{
	id fGeneralView;
	id fAudioView;
	id fVideoView;
	
	id fShowTemplateChooserRadio;
	id fUseTemplateRadio;
	id fChooseTemplateButton;
	
	id fTemplateChooserSheet;
	id fTemplateChooserView;
	id fTemplateChooserChooseButton;
	TemplateChooserViewController *templateChooserViewController;
}

- (void)setView:(NSString *)itemIdentifier;

- (void)updateGeneralView;
- (void)setUseTemplate:(BOOL)useTemplate;
- (IBAction)useTemplateDidChange:(id)sender;

- (IBAction)openTemplateChooser:(id)sender;
- (IBAction)chooseTemplate:(id)sender;
- (IBAction)closeTemplateChooser:(id)sender;

@end