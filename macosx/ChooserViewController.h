
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device or template chooser view.
 */

#import <Cocoa/Cocoa.h>

@protocol ChooserDelegate <NSObject>

@optional
- (void)chooserSelectionDidChange:(id)sender;
- (void)chooserItemWasDoubleClicked:(id)sender;

@end

@interface ChooserViewController : NSViewController
{
	IBOutlet id fVerticalSplitView;
	IBOutlet id fHorizontalSplitView;
	IBOutlet id fTableView;
	IBOutlet id fImageBrowserView;
	
	IBOutlet id fSelectedItemImageView;
	IBOutlet id fSelectedItemLabelView;
	IBOutlet id fSelectedItemDescriptionView;
	
	id<ChooserDelegate> chooserDelegate;
	
	NSMutableArray *groups;
	NSString *selectedGroup;
	NSMutableDictionary *items;
}

- (void)setDelegate:(id)theDelegate;
- (void)reloadData;

- (void)loadGroups;
- (void)loadItems;

- (void)selectGroup:(NSString *)group
	andItemWithPath:(NSString *)path;

- (NSString *)selectedGroup;
- (NSString *)selectedItemPath;

@end
