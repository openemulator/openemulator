
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations
 */

#import <Carbon/Carbon.h>

#import "DocumentController.h"

#import "Document.h"
#import "TemplateChooserWindowController.h"
#import "AudioControlsWindowController.h"
#import "LibraryWindowController.h"

#import "PAAudio.h"
#import "HIDJoystick.h"

#define LINK_HOMEPAGE	@"http://www.openemulator.org"
#define LINK_FORUMSURL	@"http://groups.google.com/group/openemulator"
#define LINK_DEVURL		@"http://code.google.com/p/openemulator"
#define LINK_DONATEURL	@"http://www.openemulator.org"

#define DONATION_NAG_TIME	(7 * (24 * (60 * 60)))

void hidDeviceWasAdded(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef device)
{
    [(DocumentController *)inContext hidDeviceWasAdded:device];
}

void hidDeviceWasRemoved(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef device)
{
    [(DocumentController *)inContext hidDeviceWasRemoved:device];
}

void hidDeviceEventOcurred(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef value)
{
    [(DocumentController *)inContext hidDeviceEventOccured:value];
}

@implementation DocumentController

- (id)init
{
    self = [super init];
    
    if (self)
    {
        paAudio = new PAAudio();
        hidJoystick = new HIDJoystick();
        
        diskImagePathExtensions = [[NSArray alloc] initWithObjects:
                                   @"bin",
                                   @"img", @"image", @"dmg", @"hdv", @"hfv", @"qcow", @"qcow2",
                                   @"iso", @"cdr",
                                   @"fdi",
                                   @"dsk", @"do", @"d13", @"po", @"cpm", @"nib", @"v2d", @"vdsk",
                                   @"2mg", @"2img",
                                   @"t64", @"tap", @"prg", @"p00",
                                   @"d64", @"d71", @"d80", @"d81", @"d82", @"x64", @"g64",
                                   @"crt",
                                   @"uef",
                                   nil];
        
        audioPathExtensions = [[NSArray alloc] initWithObjects:
                               @"wav",
                               @"aiff", @"aif", @"aifc",
                               @"au",
                               @"flac",
                               @"caf",
                               @"ogg", @"oga",
                               @"ct2",
                               nil];
        
        textPathExtensions = [[NSArray alloc] initWithObjects:
                              @"txt",
                              nil];
        
        hidDevices = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void)dealloc
{
    [diskImagePathExtensions release];
    [audioPathExtensions release];
    [textPathExtensions release];
    
    delete (PAAudio *)paAudio;
    delete (HIDJoystick *)hidJoystick;
    
    [hidDevices release];
    
    [super dealloc];
}

- (NSArray *)diskImagePathExtensions
{
    return diskImagePathExtensions;
}

- (NSArray *)audioPathExtensions
{
    return audioPathExtensions;
}

- (NSArray *)textPathExtensions
{
    return textPathExtensions;
}

- (void *)paAudio
{
    return paAudio;
}

- (void *)hidJoystick
{
    return hidJoystick;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    BOOL shaderDefault = NO;
    
    // Determine default shaderEnable value
    CGDirectDisplayID display = CGMainDisplayID();
    CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask(display);
    
    CGLRendererInfoObj info;
    GLint numRenderers = 0;
    if (CGLQueryRendererInfo(displayMask,
                             &info, 
                             &numRenderers) == 0)
    {
        for (int i = 0; i < numRenderers; i++)
        {
            GLint isAccelerated = 0;
            CGLDescribeRenderer(info, i, kCGLRPAccelerated, &isAccelerated);
            if (isAccelerated)
                shaderDefault = YES;
        }
    }
    
    // Initialize game pad callbacks
    ioHIDManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    
    NSDictionary *criterion = [NSDictionary dictionaryWithObjectsAndKeys:
                               [NSNumber numberWithInt:kHIDPage_GenericDesktop],
                               (NSString *)CFSTR(kIOHIDDeviceUsagePageKey),
                               [NSNumber numberWithInt:kHIDUsage_GD_Joystick],
                               (NSString *)CFSTR(kIOHIDDeviceUsageKey),
                               nil];
    IOHIDManagerSetDeviceMatching(ioHIDManager, (CFDictionaryRef)criterion);
    
    IOHIDManagerRegisterDeviceMatchingCallback(ioHIDManager, hidDeviceWasAdded, self);
    IOHIDManagerRegisterDeviceRemovalCallback(ioHIDManager, hidDeviceWasRemoved, self);
    IOHIDManagerRegisterInputValueCallback(ioHIDManager, hidDeviceEventOcurred, self);
    
    IOHIDManagerScheduleWithRunLoop(ioHIDManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    
    IOHIDManagerOpen(ioHIDManager, kIOHIDOptionsTypeNone);
    
    // Initialize defaults
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    
    NSDictionary *defaults = [NSDictionary dictionaryWithObjectsAndKeys:
                              [NSNumber numberWithBool:NO], @"OEAudioControlsVisible",
                              [NSNumber numberWithBool:NO], @"OEAudioFullDuplex",
                              [NSNumber numberWithFloat:1.0], @"OEAudioPlayVolume",
                              [NSNumber numberWithBool:YES], @"OEAudioPlayThrough",
                              [NSNumber numberWithBool:shaderDefault], @"OEVideoEnableShader",
                              nil
                              ];
    [userDefaults registerDefaults:defaults]; 
    
    if ([userDefaults boolForKey:@"OEAudioControlsVisible"])
        [fAudioControlsWindowController showWindow:self];
    if ([userDefaults boolForKey:@"OELibraryIsVisible"])
        [fLibraryWindowController showWindow:self];
    
    [userDefaults addObserver:self
                   forKeyPath:@"OEAudioFullDuplex"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
    [userDefaults addObserver:self
                   forKeyPath:@"OEAudioPlayVolume"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
    [userDefaults addObserver:self
                   forKeyPath:@"OEAudioPlayThrough"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
    
    ((PAAudio *)paAudio)->setFullDuplex([userDefaults
                                         boolForKey:@"OEAudioFullDuplex"]);
    ((PAAudio *)paAudio)->setPlayerVolume([userDefaults
                                           floatForKey:@"OEAudioPlayVolume"]);
    ((PAAudio *)paAudio)->setPlayerPlayThrough([userDefaults
                                                boolForKey:@"OEAudioPlayThrough"]);
    
    ((PAAudio *)paAudio)->open();
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    
    // Shamelessly ask for donations
    if (![userDefaults boolForKey:@"OEDonationFinished"])
    {
        NSDate *lastDate = [userDefaults objectForKey:@"OEDonationDate"];
        
        if (!lastDate || ([lastDate timeIntervalSinceNow] <= 0))
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:NSLocalizedString(@"Support open-source indie software",
                                                    "Donation beg -> title")];
            
            NSString *message;
            
            message = [NSString stringWithFormat:@"%@\n\n%@",
                       NSLocalizedString(@"OpenEmulator is a full-featured emulator. "
                                         "A lot of time and effort have gone into development, "
                                         "coding, and refinement. If you enjoy using it, "
                                         "please consider showing your love with a donation.",
                                         "Donation beg -> message"),
                       NSLocalizedString(@"Donate or not, there will be no difference to"
                                         " your emulator experience.",
                                         "Donation beg -> message")];
            
            [alert setInformativeText:message];
            [alert setAlertStyle:NSInformationalAlertStyle];
            
            [alert addButtonWithTitle:NSLocalizedString(@"Donate...",
                                                        "Donation beg -> button")];
            NSButton *noButton = [alert addButtonWithTitle:
                                  NSLocalizedString(@"No thanks",
                                                    "Donation beg -> button")];
            // Escape key
            [noButton setKeyEquivalent:@"\e"];
            
            // Hide the "don't show again" check the first time
            // Give them time to try the app
            [[alert suppressionButton]
             setTitle:NSLocalizedString(@"Don't bug me about this ever again.",
                                        "Donation beg -> button")];
            BOOL finished = [userDefaults boolForKey:@"OEDonationNotFirstTime"];
            if (finished)
                [alert setShowsSuppressionButton:YES];
            
            const NSInteger donateResult = [alert runModal];
            if (donateResult == NSAlertFirstButtonReturn)
                [self openDonate:self];
            
            if ([[alert suppressionButton] state] == NSOnState)
                [userDefaults setBool:YES
                               forKey:@"OEDonationFinished"];
            
            [alert release];
            
            [userDefaults setBool:YES
                           forKey:@"OEDonationNotFirstTime"];
            [userDefaults setObject:[NSDate dateWithTimeIntervalSinceNow:DONATION_NAG_TIME]
                             forKey:@"OEDonationDate"];
        }
    }
}

- (void)applicationWillTerminate:(NSNotification *)sender
{
    ((PAAudio *)paAudio)->close();
    
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setBool:[[fAudioControlsWindowController window] isVisible]
                   forKey:@"OEAudioControlsVisible"];
    [userDefaults setBool:[[fLibraryWindowController window] isVisible]
                   forKey:@"OELibraryIsVisible"];
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    // To-Do: dezoom fullscreen window?
}

- (BOOL)application:(NSApplication *)theApplication
           openFile:(NSString *)path
{
    NSString *pathExtension = [[path pathExtension] lowercaseString];
    
    // Open emulations through the Cocoa interface
    if ([pathExtension compare:@OE_PACKAGE_PATH_EXTENSION] == NSOrderedSame)
        return NO;
    
    NSWindow *window = [NSApp mainWindow];
    Document *document = nil;
    
    if (window)
        document = [[window windowController] document];
        
    if (!document)
    {
        // Open new untitled document if no document is open
        NSError *error;
        if (![self openUntitledDocumentAndDisplay:YES
                                            error:&error])
        {
            if (([[error domain] compare:NSCocoaErrorDomain] != NSOrderedSame) ||
                ([error code] != NSUserCancelledError))
                [[NSAlert alertWithError:error] runModal];
        }
        
        return YES;
    }
    
    return [self openFile:path inWindow:window];
}

- (BOOL)openFile:(NSString *)path inWindow:(NSWindow *)window
{
    NSString *pathExtension = [[path pathExtension] lowercaseString];
    
    // Open audio
    if ([audioPathExtensions containsObject:pathExtension])
    {
        [fAudioControlsWindowController readFromPath:path];
        
        return YES;
    }
    
    // Paste text
    NSWindowController *windowController = [window windowController];
    
    if ([textPathExtensions containsObject:pathExtension])
    {
        NSString *clipboard = [NSString stringWithContentsOfFile:path
                                                    usedEncoding:nil
                                                           error:nil];
        
        if ([windowController respondsToSelector:@selector(pasteString:)])
            [windowController performSelector:@selector(pasteString:)
                                   withObject:clipboard];
        
        return YES;
    }
    
    // Mount disk image
    Document *document = [windowController document];
    
    if ([diskImagePathExtensions containsObject:pathExtension])
    {
        if ([document mount:path])
            return YES;
        
        if ([document canMount:path])
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:[NSString localizedStringWithFormat:
                                   @"The document \u201C%@\u201D can't be mounted "
                                   "in this emulation.",
                                   [path lastPathComponent]]];
            [alert setInformativeText:[NSString localizedStringWithFormat:
                                       @"The storage devices compatible with this "
                                       "document are busy. "
                                       "Try unmounting a storage device."]];
            [alert runModal];
            [alert release];
            
            return YES;
        }
        
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setMessageText:[NSString localizedStringWithFormat:
                               @"The document \u201C%@\u201D can't be mounted "
                               "in this emulation.",
                               [path lastPathComponent]]];
        [alert setInformativeText:[NSString localizedStringWithFormat:
                                   @"The document is not compatible with "
                                   "any storage device in this emulation. "
                                   "Try mounting the document in another emulation."]];
        [alert runModal];
        [alert release];
        
        return YES;
    }
    
    return NO;
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    id theObject = [change objectForKey:NSKeyValueChangeNewKey];
    
    if ([keyPath isEqualToString:@"OEAudioFullDuplex"])
        ((PAAudio *)paAudio)->setFullDuplex([theObject boolValue]);
    else if ([keyPath isEqualToString:@"OEAudioPlayVolume"])
        ((PAAudio *)paAudio)->setPlayerVolume([theObject floatValue]);
    else if ([keyPath isEqualToString:@"OEAudioPlayThrough"])
        ((PAAudio *)paAudio)->setPlayerPlayThrough([theObject boolValue]);
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
    SEL action = [anItem action];
    
    if (action == @selector(newDocument:))
        return ![[fTemplateChooserWindowController window] isVisible];
    else if (action == @selector(newDocumentFromTemplateChooser:))
        return ![[fTemplateChooserWindowController window] isVisible];
    else if (action == @selector(toggleAudioControls:))
    {
        NSString *title = ([[fAudioControlsWindowController window] isVisible] ?
                           NSLocalizedString(@"Hide Audio Controls",
                                             @"Main Menu.") :
                           NSLocalizedString(@"Show Audio Controls",
                                             @"Main Menu."));
        [fAudioControlsMenuItem setTitle:title];
        
        return YES;
    }
    else if (action == @selector(toggleLibrary:))
    {
        NSString *title = ([[fLibraryWindowController window] isVisible] ?
                           NSLocalizedString(@"Hide Hardware Library",
                                             @"Main Menu.") :
                           NSLocalizedString(@"Show Hardware Library",
                                             @"Main Menu."));
        [fLibraryMenuItem setTitle:title];
        
        return YES;
    }
    
    return [super validateUserInterfaceItem:anItem];
}

- (IBAction)toggleAudioControls:(id)sender
{
    if ([[fAudioControlsWindowController window] isVisible])
        [[fAudioControlsWindowController window] orderOut:self];
    else
        [[fAudioControlsWindowController window] orderFront:self];
}

- (IBAction)toggleLibrary:(id)sender
{
    if ([[fLibraryWindowController window] isVisible])
        [[fLibraryWindowController window] orderOut:self];
    else
        [[fLibraryWindowController window] orderFront:self];
}

- (IBAction)newDocumentFromTemplateChooser:(id)sender
{
    [fTemplateChooserWindowController run];
}

- (IBAction)openDocument:(id)sender
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    
    NSMutableArray *fileTypes = [NSMutableArray array];
    [fileTypes addObject:@OE_PACKAGE_PATH_EXTENSION];
    [fileTypes addObjectsFromArray:audioPathExtensions];
    [fileTypes addObjectsFromArray:diskImagePathExtensions];
    [fileTypes addObjectsFromArray:textPathExtensions];
    
    [panel setAllowedFileTypes:fileTypes];
    
    if ([panel runModal] == NSOKButton)
    {
        NSURL *url = [panel URL];
        if ([self application:NSApp openFile:[url path]])
            return;
        
        NSError *error;
        if (![self openDocumentWithContentsOfURL:url
                                         display:YES
                                           error:&error])
            [[NSAlert alertWithError:error] runModal];
    }
}

- (id)openUntitledDocumentAndDisplay:(BOOL)displayDocument
                               error:(NSError **)outError
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if ([defaults boolForKey:@"OEDefaultTemplateEnabled"])
    {
        NSString *path = [defaults stringForKey:@"OEDefaultTemplatePath"];
        id document = nil;
        
        if (!path)
        {
            if (outError)
                *outError = [NSError errorWithDomain:@"libemulator"
                                                code:0
                                            userInfo:nil];
            return nil;
        }
        
        NSURL *url = [NSURL fileURLWithPath:path];
        if (url)
            document = [self openUntitledDocumentWithTemplateURL:url
                                                         display:displayDocument
                                                           error:nil];
        
        if (document)
            return document;
        
        [defaults setBool:NO forKey:@"OEDefaultTemplateEnabled"];
        [defaults setObject:nil forKey:@"OEDefaultTemplatePath"];
    }
    
    [self newDocumentFromTemplateChooser:self];
    
    if (outError)
        *outError = [NSError errorWithDomain:NSCocoaErrorDomain
                                        code:NSUserCancelledError
                                    userInfo:nil];
    return nil;
}

- (id)openUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
                                  display:(BOOL)displayDocument
                                    error:(NSError **)outError
{
    NSDocument *document;
    
    document = [self makeUntitledDocumentWithTemplateURL:absoluteURL
                                                   error:outError];
    if (document)
    {
        [self addDocument:document];
        
        if (displayDocument)
        {
            [document makeWindowControllers];
            
            [document showWindows];
        }
    }
    
    return document;
}

- (id)makeUntitledDocumentWithTemplateURL:(NSURL *)absoluteURL
                                    error:(NSError **)outError
{
    Document *document = [[Document alloc] initWithTemplateURL:absoluteURL
                                                         error:outError];
    if (document)
        return [document autorelease];
    
    return nil;
}

- (void)lockEmulation
{
    ((PAAudio *)paAudio)->lock();
}

- (void)unlockEmulation
{
    ((PAAudio *)paAudio)->unlock();
}

- (void)hidDeviceWasAdded:(IOHIDDeviceRef)device
{
    [hidDevices addObject:[NSValue valueWithPointer:device]];
    
    ((HIDJoystick *)hidJoystick)->addDevice();
}

- (void)hidDeviceWasRemoved:(IOHIDDeviceRef)device
{
    ((HIDJoystick *)hidJoystick)->removeDevice();
    
    [hidDevices removeObject:[NSValue valueWithPointer:device]];
}

- (void)hidDeviceEventOccured:(IOHIDValueRef)value
{
    IOHIDElementRef element = IOHIDValueGetElement(value);
    long int intValue = IOHIDValueGetIntegerValue(value);
    
    IOHIDDeviceRef device = IOHIDElementGetDevice(element);
    OEInt usagePage = IOHIDElementGetUsagePage(element);
    OEInt usageId = IOHIDElementGetUsage(element);
    long int min = IOHIDElementGetLogicalMin(element);
    long int max = IOHIDElementGetLogicalMax(element);
    
    int deviceIndex = (int) [hidDevices indexOfObject:[NSValue valueWithPointer:device]];
    
    [self lockEmulation];
    
    if (usagePage == 0x9)
        ((HIDJoystick *)hidJoystick)->setButton(deviceIndex, usageId - 1,
                                                intValue);
    else if (usagePage == 0x1)
    {
        if ((usageId >= 0x30) && (usageId <= 0x38))
        {
            float normalizedValue = ((float) intValue - min) / (max - min);
            
            ((HIDJoystick *)hidJoystick)->setAxis(deviceIndex, usageId - 0x30, normalizedValue);
        }
        else if (usageId == 0x39)
            ((HIDJoystick *)hidJoystick)->setHat(deviceIndex, usageId - 0x39, (OEInt) intValue);
    }
    
    [self unlockEmulation];
}

- (void)disableMenuBar
{
    disableMenuBarCount++;
    
    if (disableMenuBarCount == 1)
        SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
}

- (void)enableMenuBar
{
    disableMenuBarCount--;
    
    if (disableMenuBarCount == 0)
        SetSystemUIMode(kUIModeNormal, 0);
}

- (void)openHomepage:(id)sender
{
    NSURL *url = [NSURL	URLWithString:LINK_HOMEPAGE];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openForums:(id)sender
{
    NSURL *url = [NSURL	URLWithString:LINK_FORUMSURL];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openDevelopment:(id)sender
{
    NSURL *url = [NSURL	URLWithString:LINK_DEVURL];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

- (void)openDonate:(id)sender
{
    NSURL *url = [NSURL	URLWithString:LINK_DONATEURL];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

@end
