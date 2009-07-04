
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

- (BOOL)windowShouldClose:(id)window
{
	[documentController noteNewDocumentWindowClosed]; 
	return YES;
}

- (void)performChoose:(id)sender
{
	NSError *error;

	[documentController openUntitledDocumentAndDisplay:YES error:&error];
	
	[[self window] performClose:self];
}

@end
