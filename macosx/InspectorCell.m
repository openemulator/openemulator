//
//  InspectorCell.m
//  OpenEmulator
//
//  Created by Marc S. Reßl on 19/07/09.
//  Copyright 2009 ITBA. All rights reserved.
//

#import "InspectorCell.h"


@implementation InspectorCell

- (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)controlView
{
    frame.origin.y += 4.0;
    frame.size.height -= 4.0;
	
    [super drawInteriorWithFrame:frame inView:controlView];
}

@end
