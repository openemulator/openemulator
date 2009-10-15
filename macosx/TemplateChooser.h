
/**
 * OpenEmulator
 * Mac OS X Template Chooser Base Class
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

@protocol TemplateChooserDelegate <NSObject>

@optional
- (void)templateChooserSelectionDidChange:(id)sender;
- (void)templateChooserWasDoubleClicked:(id)sender;

@end

@interface TemplateChooser : NSObject
{
	id<TemplateChooserDelegate> templateChooserDelegate;
	
	id fOutlineView;
	id fChooserView;
	
	NSMutableDictionary *groups;
	NSMutableArray *groupNames;
	
	NSString *selectedGroup;
	BOOL outlineMessagesDisabled;
	BOOL chooserMessagesDisabled;
}

- (void) setDelegate:(id)theDelegate;
- (void) addTemplatesFromPath:(NSString *) path
					groupName:(NSString *) theGroupName;

- (void) populateOutlineView:(id) outlineView
			  andChooserView:(id) chooserView;
- (void) selectItemWithItemPath:(NSString *) itemPath;
- (NSString *) selectedItemPath;

@end
