
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
    NSString *title;
    NSString *subtitle;
    NSString *imagePath;
    NSString *edlPath;
	void *data;
}

- (id)initWithTitle:(NSString *)theTitle
		   subtitle:(NSString *)theSubtitle
		  imagePath:(NSString *)theImagePath
			edlPath:(NSString *)theEDLPath
			   data:(void *)theData;
- (NSString *)edlPath;
- (void *)data;

@end
