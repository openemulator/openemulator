
/**
 * OpenEmulator
 * Mac OS X Chooser Item
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an item in a device or template chooser view.
 */

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>

@interface ChooserItem : NSObject
{
	NSString *label;
	NSString *imagePath;
	
	NSString *description;
	NSString *edlPath;
	void *data;
	
	NSImage *image;
}

- (id)initWithLabel:(NSString *)theLabel
		  imagePath:(NSString *)theImagePath
		description:(NSString *)theDescription
			edlPath:(NSString *)theEDLPath
			   data:(void *)theData;
- (NSString *)description;
- (NSString *)edlPath;
- (void *)data;

@end
