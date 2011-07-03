
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
	EMULATIONITEM_AVAILABLEPORT,
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
	NSMutableArray *storages;
	
	NSString *portType;
}

- (id)initRootWithDocument:(Document *)theDocument;
- (id)initGroup:(NSString *)theUID;
- (id)initDevice:(NSString *)theUID
       component:(void *)theComponent
        portType:(NSString *)thePortType
		document:(Document *)theDocument;
- (id)initMount:(NSString *)theUID
      component:(void *)theComponent
  locationLabel:(NSString *)theLocationLabel
       document:(Document *)theDocument;
- (id)initPort:(NSString *)theUID
         label:(NSString *)theLabel
     imagePath:(NSString *)theImagePath
      portType:(NSString *)thePortType
      document:(Document *)theDocument;

- (BOOL)isGroup;
- (NSString *)uid;
- (NSMutableArray *)children;

- (NSString *)label;
- (NSImage *)image;

- (NSString *)locationLabel;
- (NSString *)stateLabel;

- (void *)device;

- (NSInteger)numberOfSettings;
- (NSString *)labelForSettingAtIndex:(NSInteger)index;
- (NSString *)typeForSettingAtIndex:(NSInteger)index;
- (NSArray *)optionsForSettingAtIndex:(NSInteger)index;
- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
- (NSString *)valueForSettingAtIndex:(NSInteger)index;

- (BOOL)isRemovable;
- (void)remove;

- (BOOL)hasCanvases;
- (void)showCanvases;

- (BOOL)hasStorages;
- (BOOL)mount:(NSString *)path;
- (BOOL)canMount:(NSString *)path;
- (BOOL)isMount;
- (void)revealInFinder;
- (BOOL)isLocked;
- (void)unmount;

- (BOOL)isPort;
- (BOOL)addOEDocument:(NSString *)path;
- (NSString *)portType;

@end
