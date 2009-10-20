
/**
 * OpenEmulator
 * Mac OS X Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a dml chooser controller.
 */

#import <Cocoa/Cocoa.h>

@protocol ChooserDelegate <NSObject>

@optional
- (void)chooserWasDoubleClicked:(id)sender;

@end

@interface ChooserController : NSViewController
{
	id fOutlineView;
	id fImageBrowserView;
	
	id<ChooserDelegate> chooserDelegate;
	
	NSMutableDictionary *groups;
	NSMutableArray *groupNames;
	
	NSString *selectedGroup;
}

- (id) initWithTemplates;
- (id) initWithDevices;
- (void) setDelegate:(id)theDelegate;
- (void) updateUserTemplates;
- (void) addTemplatesFromPath:(NSString *) path
					groupName:(NSString *) theGroupName;
- (void) selectItemWithItemPath:(NSString *) itemPath;
- (NSString *) selectedItemPath;

@end
