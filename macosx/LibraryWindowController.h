
/**
 * OpenEmulator
 * Mac OS X Library Window Controller
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the library window.
 */

#import <Cocoa/Cocoa.h>

#import "LibraryTableCell.h"

@interface LibraryWindowController : NSWindowController
<NSTableViewDataSource, NSTableViewDelegate>
{
	IBOutlet id fSplitView;
	IBOutlet NSTableView *fTableView;

    IBOutlet id fSelImage;
    IBOutlet id fSelLabel;
    IBOutlet id fSelType;
    IBOutlet id fSelDescription;

    LibraryTableCell *cell;
    
	NSMutableArray *items;
}

@end
