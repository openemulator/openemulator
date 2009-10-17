
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
}

- (void) setDelegate:(id)theDelegate;
- (void) updateUserTemplates;
- (void) addTemplatesFromPath:(NSString *) path
					groupName:(NSString *) theGroupName;
- (void) setupOutlineView:(id) outlineView
		   andChooserView:(id) chooserView;
- (void) selectItemWithItemPath:(NSString *) itemPath;
- (NSString *) selectedItemPath;

@end
