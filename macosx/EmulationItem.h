
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item.
 */

#import <Cocoa/Cocoa.h>

#import "Document.h"

@interface EmulationItem : NSObject
{
	Document *document;
	
	NSString *uid;
	NSString *label;
	NSImage *image;
	NSString *location;
	NSString *state;
	BOOL showable;
	BOOL mountable;
	BOOL mounted;
	
	NSMutableArray *settingsRefs;
	NSMutableArray *settingsNames;
	NSMutableArray *settingsLabels;
	NSMutableArray *settingsTypes;
	NSMutableArray *settingsOptions;
	
	NSMutableArray *children;
}

- (id)initWithDocument:(Document *)theDocument;
- (id)initWithLabel:(NSString *)theLabel;
- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument;
- (id)initWithDiskImageAtPath:(NSString *)thePath
			 storageComponent:(void *)theStorageComponent
					 location:(NSString *)theLocation
					 readOnly:(BOOL)isReadOnly
					   locked:(BOOL)isLocked;

- (NSImage *)getImage:(NSString *)path;

- (NSString *)uid;
- (NSImage *)image;
- (NSString *)label;
- (NSString *)location;
- (NSString *)state;
- (BOOL)showable;
- (BOOL)mountable;
- (BOOL)mounted;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;

- (NSMutableArray *)children;
- (EmulationItem *)childWithUid:(NSString *)theUid;

@end
