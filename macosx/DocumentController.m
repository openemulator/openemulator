
/**
 * OpenEmulator
 * Mac OS X Document Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls emulations.
 */

#import <Carbon/Carbon.h>

#import "Document.h"
#import "DocumentController.h"
#import "TemplateChooserController.h"

#import "oepa_c.h"

@implementation DocumentController

- (id) init
{
	if (self = [super init])
	{
		fileTypes = [[NSArray alloc] initWithObjects:
					 @"emulation",
					 @"aiff", @"aif", @"wav",
					 @"dsk", @"do", @"d13", @"po", @"cpm", @"nib", @"v2d",
					 @"vdsk", @"2mg", @"2img", @"hdv", @"sdk",
					 @"d64", @"g64", @"d71", @"d81", @"t64",
					 @"tap", @"prg", @"p00", @"crt",
					 @"fdi",
					 @"img", @"hdv", @"hfv",
					 @"iso", @"cdr",
					 nil];
		
		disableMenuBarCount = 0;
		
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		[defaults addObserver:self
				   forKeyPath:@"values.OEFullDuplex"
					  options:NSKeyValueObservingOptionNew
					  context:NULL];
	}
	
	return self;
}

- (void) dealloc
{
	[fileTypes release];
	
	[super dealloc];
}

- (void) applicationWillFinishLaunching:(NSNotification *) notification
{
	[fInspectorController restoreWindowState:self];
	
	c_oepaOpen();
}

- (BOOL) application:(NSApplication *) theApplication
			openFile:(NSString *) filename
{
	if ([[filename pathExtension] compare:@"emulation"] == NSOrderedSame)
		return NO;
	
	if (![self currentDocument])
	{
		NSError *error;
		if (![self openUntitledDocumentAndDisplay:YES
											error:&error])
		{
			if (([[error domain] compare:NSCocoaErrorDomain] != NSOrderedSame) ||
				([error code] != NSUserCancelledError))
				[[NSAlert alertWithError:error] runModal];
			
			return YES;
		}
	}
	
	// To-Do: Mount disk image
	
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert setMessageText:[NSString localizedStringWithFormat:
						   @"The document \u201C%@\u201D could not be opened. "
						   "This emulation cannot open files in this format.",
						   [filename lastPathComponent]]];
	[alert setAlertStyle:NSCriticalAlertStyle];
	[alert runModal];
	
	return YES;
}

- (void) applicationDidFinishLaunching:(NSNotification *) notification
{
}

- (void) applicationWillTerminate:(NSNotification *) notification
{
	c_oepaClose();
	
	[fInspectorController storeWindowState:self];
}

- (void) observeValueForKeyPath:(NSString *) keyPath
					   ofObject:(id) object
						 change:(NSDictionary *) change
                        context:(void *) context
{
	NSLog(@"Observed.");
	if ([keyPath isEqualToString:@"values.OEFullDuplex"])
	{
		id object = [change objectForKey:NSKeyValueChangeNewKey];
		int value = [object intValue];
		c_oepaSetFullDuplex(value);
	}
}

- (BOOL) validateUserInterfaceItem:(id) item
{
	if ([item action] == @selector(newDocument:))
		return ![[fTemplateChooserController window] isVisible];
	else if ([item action] == @selector(newDocumentFromTemplateChooser:))
		return ![[fTemplateChooserController window] isVisible];
	else
		return YES;
}

- (IBAction) newDocumentFromTemplateChooser:(id) sender
{
	[fTemplateChooserController run];
}

- (IBAction) openDocument:(id) sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	if ([panel runModalForTypes:fileTypes] == NSOKButton)
	{
		NSURL *url = [panel URL];
		if ([self application:NSApp openFile:[url path]])
			return;
		
		NSError *error;
		if (![self openDocumentWithContentsOfURL:url display:YES error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

- (id) openUntitledDocumentAndDisplay:(BOOL) displayDocument error:(NSError **) outError
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if (![defaults boolForKey:@"OEUseDefaultTemplate"])
	{
		[self newDocumentFromTemplateChooser:self];
		
		*outError = [NSError errorWithDomain:NSCocoaErrorDomain
										code:NSUserCancelledError
									userInfo:nil];
		return nil;
	}
	else
	{
		NSString *defaultTemplate = [defaults stringForKey:@"OEDefaultTemplate"];
		id document = nil;
		
		if (!defaultTemplate)
		{
			*outError = [NSError errorWithDomain:@"libemulator"
											code:0
										userInfo:nil];
			return nil;
		}
		
		NSURL *absoluteURL = [NSURL fileURLWithPath:defaultTemplate];
		document = [self openUntitledDocumentWithTemplateURL:absoluteURL
													 display:displayDocument
													   error:outError];
		return document;
	}
}

- (id) openUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
								   display:(BOOL) displayDocument
									 error:(NSError **) outError
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

- (id) makeUntitledDocumentWithTemplateURL:(NSURL *) absoluteURL
									 error:(NSError **) outError
{
	Document *document = [[Document alloc] initWithTemplateURL:absoluteURL
														 error:outError];
	if (document)
		return [document autorelease];
	
	return nil;
}

- (void) disableMenuBar
{
	disableMenuBarCount++;
	
	if (disableMenuBarCount == 1)
		SetSystemUIMode(kUIModeAllHidden, kUIOptionAutoShowMenuBar);
}

- (void) enableMenuBar
{
	disableMenuBarCount--;
	
	if (disableMenuBarCount == 0)
		SetSystemUIMode(kUIModeNormal, 0);
}

- (void) linkHomepage:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_HOMEPAGE];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) linkForums:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_FORUMSURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) linkDevelopment:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DEVURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (void) linkDonate:(id) sender
{
	NSURL *url = [NSURL	URLWithString:LINK_DONATEURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

@end
