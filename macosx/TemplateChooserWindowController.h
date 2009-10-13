
/**
 * OpenEmulator
 * Mac OS X Template Chooser Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

#import "DocumentController.h"

@interface TemplateChooserWindowController : NSWindowController
{
	IBOutlet id fOutlineView;
	IBOutlet id fChooserView;
	
	DocumentController *documentController;
	
	NSMutableDictionary *groups;
	NSMutableArray *groupNames;
	
	NSString *selectedGroup;
}

- (id) init:(DocumentController *) theDocumentController;
- (void) addTemplatesFromPath:(NSString *) path
			   forceGroupName:(NSString *) forcedGroupName;
- (void) performChoose:(id) sender;

@end
