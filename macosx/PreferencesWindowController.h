
/**
 * OpenEmulator
 * Mac OS X Preferences Window Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences window
 */

#import <Cocoa/Cocoa.h>

@class TemplateChooserViewController;

@interface PreferencesWindowController : NSWindowController <NSToolbarDelegate>
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

- (void)updateGeneralView;
- (void)setUseTemplate:(BOOL)useTemplate;
- (IBAction)useTemplateDidChange:(id)sender;

- (IBAction)openTemplateChooser:(id)sender;
- (IBAction)chooseTemplate:(id)sender;
- (IBAction)closeTemplateChooser:(id)sender;

@end
