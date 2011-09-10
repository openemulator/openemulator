
/**
 * OpenEmulator
 * Mac OS X Template Chooser Item
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser item
 */

#import <Cocoa/Cocoa.h>

@interface TemplateChooserItem : NSObject
{
    NSString *path;
    BOOL loaded;
    
    NSString *label;
    NSImage *image;
    NSString *description;
}

- (id)initWithOEDocumentPath:(NSString *)thePath;

- (NSString *)description;
- (NSString *)path;

@end
