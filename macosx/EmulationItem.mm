
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item.
 */

#import "EmulationItem.h"
#import "StringConversion.h"

#import "OEEmulation.h"

#import "DeviceInterface.h"

@implementation EmulationItem

- (id)initWithDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_ROOT;
		children = [[NSMutableArray alloc] init];
		
		OEComponents *devices = (OEComponents *)[theDocument getDevices];
		for (NSInteger i = 0; i < devices->size(); i++)
		{
			OEComponent *device = devices->at(i);
			
			string group;
			device->postMessage(NULL, DEVICE_GET_GROUP, &group);
			
			// Create group item
			NSString *groupName = getNSString(group);
			EmulationItem *groupItem = [self childWithUid:groupName];
			if (!groupItem)
			{
				groupItem = [[EmulationItem alloc] initWithGroupName:groupName];
				[children addObject:groupItem];
				[groupItem release];
			}
			
			// Create device item
			EmulationItem *deviceItem;
			deviceItem = [[EmulationItem alloc] initWithDevice:device
													  document:theDocument];
			[[groupItem children] addObject:deviceItem];
			[deviceItem release];
		}
	}
	
	return self;
}

- (id)initWithGroupName:(NSString *)theGroupName
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_GROUP;
		children = [[NSMutableArray alloc] init];
		
		label = [[NSLocalizedString(theGroupName, @"Emulation Item Label.")
				  uppercaseString] retain];
	}
	
	return self;
}

- (id)initWithDevice:(void *)theDevice
			document:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_DEVICE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		OEComponent *device = (OEComponent *)theDevice;
		
		label = [getNSString(deviceInfo->label) retain];
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [[resourcePath stringByAppendingPathComponent:
								getNSString(deviceInfo->image)] retain];
		image = [[NSImage alloc] initByReferencingFile:imagePath];
		
		storage = deviceInfo->storage;
		if (storage && ([document imagePathForStorage:storage])
		{
			NSString *theUID;
			
			theUID = [NSString stringWithFormat:@"%@.storage", uid];
			
			EmulationItem *storageItem;
			storageItem = [[EmulationItem alloc] initWithStorage:storage
															 uid:theUID 
														location:location
														document:theDocument];
			[children addObject:storageItem];
			[storageItem release];
		}
	}
	
	return self;
}

- (id)initWithStorage:(void *)theStorage
			 location:(NSString *)theLocation
			 document:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_DISKIMAGE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		label = [[[document imagePathForStorage:theStorage]
				  lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[children release];
	
	[label release];
	
	[super dealloc];
}

- (EmulationItemType)type
{
	return itemType;
}

- (NSMutableArray *)children
{
	return children;
}

- (EmulationItem *)childWithUid:(NSString *)theUid
{
	for (EmulationItem *item in children)
	{
		if ([[item uid] compare:theUid] == NSOrderedSame)
			return item;
	}
	
	return nil;
}

- (NSImage *)image
{
	return image;
}

- (NSString *)label
{
	return label;
}

- (NSString *)location
{
	return location;
}

- (NSString *)state
{
	if (itemType == EMULATION_ITEM_DISKIMAGE)
	{
		NSString *theState;
		theState = ([document isStorageWritable:storage] ?
					NSLocalizedString(@"Read/Write", @"Emulation Item.") : 
					NSLocalizedString(@"Read-Only", @"Emulation Item."));
		if ([document isStorageLocked:storage])
			theState = [theState stringByAppendingString:
						NSLocalizedString(@", Locked", @"Emulation Item.")];
		
		return theState;
	}
	
	return state;
}

- (BOOL)isRemovable
{
	return (itemType == EMULATION_ITEM_DEVICE) && [location length];
}



- (BOOL)isCanvas
{
	return ([canvases count] != 0);
}

- (void)showCanvases
{
	for (NSInteger i = 0; i < [canvases count]; i++)
		[document showCanvas:[[canvases objectAtIndex:i] pointerValue]];
}



- (BOOL)isStorage
{
	return (((itemType == EMULATION_ITEM_DEVICE) && storage) ||
			![location length]);
}

- (BOOL)isMounted
{
	return (itemType == EMULATION_ITEM_DISKIMAGE);
}

- (BOOL)isLocked
{
	return [document isStorageLocked:storage];
}

- (NSString *)diskImagePath
{
	return [document imagePathForStorage:storage];
}

- (BOOL)mount:(NSString *)path
{
	if (![location length])
		return [document mount:path];
	
	return [document mount:path inStorage:storage];
}

- (void)unmount
{
	[document unmount:storage];
}

- (BOOL)canMount:(NSString *)path
{
	if (![location length])
		return [document canMount:path];
	
	return [document canMount:path inStorage:storage];
}



- (NSInteger)numberOfSettings
{
	DeviceSettings settings;
	
	component->postMessage(NULL, DEVICE_GET_SETTINGS, settings)
	return 
	return [settingsRefs count];
}

- (NSString *)labelForSettingAtIndex:(NSInteger)index
{
	return [settingsLabels objectAtIndex:index];
}

- (NSString *)typeForSettingAtIndex:(NSInteger)index
{
	return [settingsTypes objectAtIndex:index];
}

- (NSArray *)optionsForSettingAtIndex:(NSInteger)index
{
	return [settingsOptions objectAtIndex:index];
}

- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
{
	NSString *ref = [settingsRefs objectAtIndex:index];
	NSString *name = [settingsNames objectAtIndex:index];
	
	NSString *type = [settingsTypes objectAtIndex:index];
	if ([type compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [options objectAtIndex:[value integerValue]];
	}
	
	[document setValue:value ofProperty:name forComponent:ref];
}

- (NSString *)valueForSettingAtIndex:(NSInteger)index
{
	NSString *ref = [settingsRefs objectAtIndex:index];
	NSString *name = [settingsNames objectAtIndex:index];
	
	NSString *value = [document valueOfProperty:name forComponent:ref];
	
	NSString *type = [settingsTypes objectAtIndex:index];
	if ([type compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [NSString stringWithFormat:@"%d", [options indexOfObject:value]];
	}
	
	return value;
}

@end
