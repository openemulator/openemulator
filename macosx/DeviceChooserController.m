
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

- (void) updateDeviceChooser
{
	[deviceChooserViewController selectItemWithPath:nil];
	
	[[self window] center];
	
	[deviceChooserViewController updateForInlets:nil];

	[fPreviousButton setEnabled:NO];
	[fNextButton setEnabled:([deviceChooserViewController selectedItemPath]
							 != nil)];
	
	[fMessage setStringValue:
	 NSLocalizedString(@"Choose a template for your new expansion:",
					   @"Choose a template for your new expansion")];
}

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	[self setWindowFrameAutosaveName:@"DeviceChooser"];
	
	NSView *view = [deviceChooserViewController view];
	[fChooserView addSubview:view];
	[view setFrameSize:[fChooserView frame].size];
	
	[self updateDeviceChooser];
}

- (void) showWindow:(id) sender
{
	[super showWindow:sender];
	
	[self updateDeviceChooser];
}

- (void) runModal:(id) sender
{
	[NSApp runModalForWindow:[self window]];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self performNext:sender];
}

- (IBAction) performCancel:(id) sender
{
	[NSApp abortModal];
	[[self window] orderOut:self];
}

- (IBAction) performPrevious:(id) sender
{
	[NSApp abortModal];
	[[self window] orderOut:self];
}

- (IBAction) performNext:(id) sender
{
	NSString *itemPath = [deviceChooserViewController selectedItemPath];
	NSURL *url = nil;
	if (itemPath)
		url = [NSURL fileURLWithPath:itemPath];
	
	[NSApp stopModal];
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
