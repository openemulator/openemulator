
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import "TemplateChooserWindowController.h"

@implementation TemplateChooserWindowController

- (id)init:(DocumentController *)theDocumentController
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	if (self)
	{
		documentController = theDocumentController;
	}
	
	return self;
}

- (id)init
{
	return [self init:nil];
}

- (void)windowDidLoad
{
	// Reset window size, leave window centered on screen
}

- (void)windowWillClose:(NSNotification *)notification
{
	if (documentController)
		[documentController noteTemplateChooserWindowClosed]; 
}

- (void)performChoose:(id)sender
{
	NSError *error;
	NSURL *url = [NSURL fileURLWithPath:@"/Users/mressl/Documents/OpenEmulator/openemulator/build/Debug/OpenEmulator.app/Contents/Resources/templates/Apple II.emulation"];
	
	[documentController openUntitledDocumentFromTemplateURL:url
													  error:&error];
	
	[[self window] performClose:self];
}

@end
