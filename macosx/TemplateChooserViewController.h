
/**
 * OpenEmulator
 * Mac OS X Template Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser view.
 */

#import <Cocoa/Cocoa.h>

#import "VerticallyCenteredTextFieldCell.h"

@protocol TemplateChooserDelegate <NSObject>

@optional

- (void)templateChooserSelectionDidChange:(id)sender;
- (void)templateChooserItemWasDoubleClicked:(id)sender;

@end

@interface TemplateChooserViewController : NSViewController
<NSComboBoxDataSource>
{
	IBOutlet id fVerticalSplitView;
	IBOutlet id fHorizontalSplitView;
	IBOutlet id fTableView;
	IBOutlet id fImageBrowserView;
	
	IBOutlet id fSelectedItemImageView;
	IBOutlet id fSelectedItemLabelView;
	IBOutlet id fSelectedItemDescriptionView;
	
	id < TemplateChooserDelegate > delegate;
	
	VerticallyCenteredTextFieldCell *tableCell;
	
	NSMutableArray *groups;
	NSMutableDictionary *items;
	
	NSString *selectedGroup;
}

- (void)loadGroups;
- (void)loadItems;

- (void)setDelegate:(id)theDelegate;
- (void)reloadData;

- (void)selectGroup:(NSString *)group
	andItemWithPath:(NSString *)path;

- (NSString *)selectedGroup;
- (NSString *)selectedItemPath;

- (BOOL)templatesAtPathValid:(NSString *)groupPath;
- (void)addTemplatesAtPath:(NSString *)groupPath
				   toGroup:(NSString *)group;

@end
