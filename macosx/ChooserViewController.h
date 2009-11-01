
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a dml chooser view controller.
 */

#import <Cocoa/Cocoa.h>

@protocol ChooserDelegate <NSObject>

@optional
- (void)chooserWasDoubleClicked:(id)sender;

@end

@interface ChooserViewController : NSViewController
{
	id fOutlineView;
	id fImageBrowserView;
	
	id<ChooserDelegate> chooserDelegate;
	
	NSMutableDictionary *groups;
	NSMutableArray *groupNames;
	
	NSString *selectedGroup;
}

- (void) setDelegate:(id)theDelegate;

- (void) selectItemWithPath:(NSString *) itemPath;
- (NSString *) selectedItemPath;

@end
