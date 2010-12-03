
/**
 * OpenEmulator
 * Mac OS X Template Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser view.
 */

#import <Cocoa/Cocoa.h>

#import "ChooserViewController.h"

@interface TemplateChooserViewController : ChooserViewController
{
}

- (void)updateUserTemplates;
- (void)addTemplatesFromPath:(NSString *)path
					 toGroup:(NSString *)groupName;
- (void)removeGroup:(NSString *)groupName;

@end
