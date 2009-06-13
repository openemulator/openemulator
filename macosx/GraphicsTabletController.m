//
//  AppleGraphicsTabletController.m
//  OpenEmulator
//
//  Created by Marc S. Reßl on 31/05/09.
//  Copyright 2009 ITBA. All rights reserved.
//

#import "GraphicsTabletController.h"


@implementation GraphicsTabletController

- (id)init
{
	return [super initWithWindowNibName:@"GraphicsTablet"];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	NSPanel *panel = (NSPanel *)[self window];
	[panel setFrameUsingName:@"GraphicsTablet"];
	[panel setFrameAutosaveName:@"GraphicsTablet"];
}

@end
