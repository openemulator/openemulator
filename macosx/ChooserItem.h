
/**
 * OpenEmulator
 * Mac OS X Chooser Item
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an chooser item for template and device choosing.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

@interface ChooserItem : NSObject
{
    NSString *itemPath;
    NSString *label;
    NSString *imagePath;
    NSString *description;
}

- (id) initWithItem:(NSString *) theItemPath
			  label:(NSString *) theLabel
		  imagePath:(NSString *) theImagePath
		description:(NSString *) theDescription;
- (NSString *) itemPath;

@end
