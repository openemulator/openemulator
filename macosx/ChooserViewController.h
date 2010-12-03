
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
- (void)chooserWasDoubleClicked:(id)sender;

@end

@interface ChooserViewController : NSViewController
{
	id fOutlineView;
	id fImageBrowserView;
	
	id<ChooserDelegate> chooserDelegate;
	
	NSMutableArray *groups;
	NSString *selectedGroup;
	
	NSMutableDictionary *items;
}

- (void)setDelegate:(id)theDelegate;

- (void)selectItemWithPath:(NSString *)itemPath;
- (NSString *)selectedItemPath;

@end
