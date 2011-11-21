
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls all emulations
 */

#import <Cocoa/Cocoa.h>

#import <IOKit/hid/IOHIDManager.h>

@class AudioControlsWindowController;
@class TemplateChooserWindowController;
@class LibraryWindowController;

@interface DocumentController : NSDocumentController
<NSApplicationDelegate>
{
    IBOutlet AudioControlsWindowController *fAudioControlsWindowController;
    IBOutlet TemplateChooserWindowController *fTemplateChooserWindowController;
    IBOutlet LibraryWindowController *fLibraryWindowController;
    
    IBOutlet id fAudioControlsMenuItem;
    IBOutlet id fLibraryMenuItem;
    
    NSArray *diskImagePathExtensions;
    NSArray *audioPathExtensions;
    NSArray *textPathExtensions;
    
    IOHIDManagerRef ioHIDManager;
    
    void *paAudio;
    void *hidJoystick;
    
    NSMutableArray *hidDevices;
    
    NSInteger disableMenuBarCount;
}

- (NSArray *)diskImagePathExtensions;
- (NSArray *)audioPathExtensions;
- (NSArray *)textPathExtensions;
- (void *)paAudio;
- (void *)hidJoystick;

- (IBAction)toggleAudioControls:(id)sender;
- (IBAction)toggleLibrary:(id)sender;

- (IBAction)newDocumentFromTemplateChooser:(id)sender;
- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
                                  display:(BOOL)displayDocument
                                    error:(NSError **)outError;
- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
                                    error:(NSError **)outError;

- (void)hidDeviceWasAdded:(IOHIDDeviceRef)device;
- (void)hidDeviceWasRemoved:(IOHIDDeviceRef)device;
- (void)hidDeviceEventOccured:(IOHIDValueRef)value;

- (void)disableMenuBar;
- (void)enableMenuBar;

- (IBAction)openHomepage:(id)sender;
- (IBAction)openForums:(id)sender;
- (IBAction)openDevelopment:(id)sender;
- (IBAction)openDonate:(id)sender;

@end
