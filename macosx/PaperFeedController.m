
/**
 * OpenEmulator
 * Mac OS X Paper Feed Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the paper feed.
 */

#import "PaperFeedController.h"

@implementation PaperFeedController

- (id) init
{
	self = [super initWithWindowNibName:@"PaperFeed"];
	
	return self;
}

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	NSPanel *panel = (NSPanel *) [self window];
	[panel setFrameUsingName:@"PaperFeed"];
	[panel setFrameAutosaveName:@"PaperFeed"];
}

@end
