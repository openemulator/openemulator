
/**
 * OpenEmulator
 * Mac OS X Info Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation's info window.
 */

#import "InfoWindowController.h"

@implementation InfoWindowController

- (id) init
{
	self = [self initWithWindowNibName:@"Info"];
	
	return self;
}

- (void) windowDidLoad
{
	documentController = [NSDocumentController sharedDocumentController];
	document = [self document];
}

@end
