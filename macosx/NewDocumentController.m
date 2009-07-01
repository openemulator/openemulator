
/**
 * OpenEmulator
 * Mac OS X New Document Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the new document window.
 */

#import "NewDocumentController.h"

@implementation NewDocumentController

- (id)init
{
	self = [super initWithWindowNibName:@"NewDocument"];
	if (self)
	{
		[self showWindow:self];
	}
	
	return self;
}

- (void)windowDidLoad
{
}

@end
