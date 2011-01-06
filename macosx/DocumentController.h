
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls all emulations.
 */

#import <Cocoa/Cocoa.h>

@class AudioControlsWindowController;
@class TemplateChooserWindowController;
@class LibraryWindowController;

@interface DocumentController : NSDocumentController
{
	IBOutlet AudioControlsWindowController *fAudioControlsWindowController;
	IBOutlet TemplateChooserWindowController *fTemplateChooserWindowController;
	IBOutlet LibraryWindowController *fLibraryWindowController;
	
	IBOutlet id fAudioControlsMenuItem;
	IBOutlet id fLibraryMenuItem;
	
	NSArray *diskImagePathExtensions;
	NSArray *audioPathExtensions;
	NSArray *textPathExtensions;
	
	void *portAudioHAL;
	
	int disableMenuBarCount;
}

- (NSArray *)diskImagePathExtensions;
- (NSArray *)audioPathExtensions;
- (NSArray *)textPathExtensions;
- (void *)portAudioHAL;

- (IBAction)toggleAudioControls:(id)sender;
- (IBAction)toggleLibrary:(id)sender;

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
								  display:(BOOL)displayDocument
									error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
									error:(NSError **)outError;

- (void)disableMenuBar;
- (void)enableMenuBar;

- (IBAction)openHomepage:(id)sender;
- (IBAction)openForums:(id)sender;
- (IBAction)openDevelopment:(id)sender;
- (IBAction)openDonate:(id)sender;

@end
