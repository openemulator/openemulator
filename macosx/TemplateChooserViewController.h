
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

- (BOOL)validTemplatesAtPath:(NSString *)groupPath;
- (void)addGroupAtPath:(NSString *)groupPath
			   toGroup:(NSString *)group;

@end
