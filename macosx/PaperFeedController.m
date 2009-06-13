//
//  PaperFeedController.m
//  OpenEmulator
//
//  Created by Marc S. Reßl on 31/05/09.
//  Copyright 2009 ITBA. All rights reserved.
//

#import "PaperFeedController.h"


@implementation PaperFeedController

- (id)init
{
	return [super initWithWindowNibName:@"PaperFeed"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	NSPanel *panel = (NSPanel *)[self window];
	[panel setFrameUsingName:@"PaperFeed"];
	[panel setFrameAutosaveName:@"PaperFeed"];
}

@end
