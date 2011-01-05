
/**
 * OpenEmulator
 * Mac OS X Template Chooser Item
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser item.
 */

#import <Cocoa/Cocoa.h>

@interface TemplateChooserItem : NSObject
{
	NSString *edlPath;
	BOOL edlLoaded;
	
	NSString *label;
	NSImage *image;
	NSString *description;
}

- (id)initWithEDLPath:(NSString *)path;

- (NSString *)description;
- (NSString *)edlPath;

@end
