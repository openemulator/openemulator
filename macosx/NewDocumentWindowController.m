
/**
 * OpenEmulator
 * Mac OS X New Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the new document window.
 */

#import "NewDocumentWindowController.h"

@implementation NewDocumentWindowController

- (id)init:(DocumentController*)theDocumentController
{
	self = [super initWithWindowNibName:@"NewDocument"];
	if (self)
	{
		documentController = theDocumentController;
	}
	
	return self;
}

- (void)windowWillClose:(NSNotification*)notification
{
	[documentController noteNewDocumentWindowClosed]; 
}

- (void)performChoose:(id)sender
{
	NSError *error;
	NSString *url = [NSURL fileURLWithPath:@"/tmp/temp.emulation"];
	
	[documentController openUntitledDocumentFromTemplateURL:url
													  error:&error];
	
	[[self window] performClose:self];
}

@end
