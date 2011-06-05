
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
    IBOutlet id fPathFilter;
    IBOutlet id fSearchFilter;
    
	IBOutlet id fSplitView;
	IBOutlet NSTableView *fTableView;

    NSMutableArray *filterPaths;
	NSMutableArray *items;
	NSMutableArray *filteredItems;
    LibraryTableCell *cell;
    
    IBOutlet id fSelImage;
    IBOutlet id fSelLabel;
    IBOutlet id fSelType;
    IBOutlet id fSelDescription;
}

- (IBAction)filterItems:(id)sender;

@end
