
/**
 * OpenEmulator
 * Mac OS X Device Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the new document from template window.
 */

#import "DeviceChooserController.h"

@implementation DeviceChooserController

- (id) init
{
	self = [super initWithWindowNibName:@"DeviceChooser"];
	
	if (self)
	{
		deviceChooserViewController = [[DeviceChooserViewController alloc] init];
		[deviceChooserViewController setDelegate:self];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[deviceChooserViewController release];
}

- (void) windowDidLoad
{
	[self setWindowFrameAutosaveName:@"DeviceChooser"];
	
	NSView *view = [deviceChooserViewController view];
	[fChooserView addSubview:view];
	[view setFrame:[fChooserView bounds]];
	
	[[self window] center];
	
//	[deviceChooserViewController selectItemWithItemPath:nil];
}

- (void) showWindow:(id) sender
{
	[[self window] center];
	
	[super showWindow:sender];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self chooseTemplate:sender];
}

- (IBAction) chooseTemplate:(id) sender
{
	NSString *itemPath = [deviceChooserViewController selectedItemPath];
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setObject:itemPath
					 forKey:@"OELastTemplate"];
	
	NSURL *url = [NSURL fileURLWithPath:itemPath];
	
	[[self window] orderOut:self];
	
	if (url)
	{
/*		NSError *error;
		if (![fDocumentController openUntitledDocumentFromTemplateURL:url
															  display:YES
																error:&error])
			[[NSAlert alertWithError:error] runModal];*/
	}
}

@end
