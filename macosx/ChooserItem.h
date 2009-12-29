
/**
 * OpenEmulator
 * Mac OS X Chooser Item
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an item in a device or template chooser view.
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
