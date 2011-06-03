
/**
 * OpenEmulator
 * Mac OS X Library Window Controller
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the library window.
 */

#import <Cocoa/Cocoa.h>

@interface LibraryWindowController : NSWindowController
<NSTableViewDataSource>
{
	IBOutlet NSTableView *fTableView;
	
	NSMutableArray *items;
}

@end
