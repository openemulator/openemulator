
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

typedef enum
{
	EMULATIONITEM_ROOT,
	EMULATIONITEM_GROUP,
	EMULATIONITEM_DEVICE,
	EMULATIONITEM_MOUNT,
} EmulationItemType;

@interface EmulationItem : NSObject
{
	EmulationItemType type;
	NSString *uid;
	NSMutableArray *children;
	Document *document;
	
	NSString *label;
	NSImage *image;
	
	NSString *locationLabel;
	NSString *stateLabel;
	
	void *device;
	
	NSMutableArray *settingsRef;
	NSMutableArray *settingsName;
	NSMutableArray *settingsLabel;
	NSMutableArray *settingsType;
	NSMutableArray *settingsOptions;
	
	NSMutableArray *canvases;
	
	void *storage;
}

- (id)initRootWithDocument:(Document *)theDocument;
- (id)initGroup:(NSString *)theGroupName;
- (id)initDevice:(void *)theDevice
			 uid:(NSString *)theUID
		document:(Document *)theDocument;
- (id)initMountWithStorage:(void *)theStorage
					   uid:(NSString *)theUID
			 locationLabel:(NSString *)theLocationLabel
				  document:(Document *)theDocument;

- (BOOL)isGroup;
- (NSString *)uid;
- (NSMutableArray *)children;
- (EmulationItem *)childWithUID:(NSString *)theUID;

- (NSString *)label;
- (NSImage *)image;

- (NSString *)locationLabel;
- (NSString *)stateLabel;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

- (BOOL)hasCanvases;
- (void)showCanvases;

- (BOOL)isStorageDevice;
- (BOOL)mount:(NSString *)path;
- (BOOL)testMount:(NSString *)path;

- (BOOL)isMount;
- (void)revealInFinder;
- (BOOL)isLocked;
- (void)unmount;

@end
