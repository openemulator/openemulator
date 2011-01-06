
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

#import "Emulation.h"

@implementation EmulationItem

- (id)initWithDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_ROOT;
		children = [[NSMutableArray alloc] init];
		
		EmulationInfo *emulationInfo = (EmulationInfo *)[theDocument emulationInfo];
		for (NSInteger i = 0; i < emulationInfo->size(); i++)
		{
			EmulationDeviceInfo &deviceInfo = emulationInfo->at(i);
			
			// Create group item
			NSString *groupName = getNSString(deviceInfo.group);
			EmulationItem *groupItem = [self childWithUid:groupName];
			if (!groupItem)
			{
				groupItem = [[EmulationItem alloc] initWithGroupName:groupName];
				[children addObject:groupItem];
				[groupItem release];
			}
			
			// Create device item
			EmulationItem *deviceItem;
			deviceItem = [[EmulationItem alloc] initWithDeviceInfo:&deviceInfo
														inDocument:theDocument];
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
		
		uid = [theGroupName copy];
		label = [[NSLocalizedString(theGroupName, @"Emulation Item Label.")
				  uppercaseString] retain];
	}
	
	return self;
}

- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_DEVICE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [[resourcePath stringByAppendingPathComponent:
								getNSString(deviceInfo->image)] retain];
		image = [[NSImage alloc] initByReferencingFile:imagePath];
		
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		
		canvasComponents = [[NSMutableArray alloc] init];
		OEComponents &canvases = deviceInfo->canvases;
		for (OEComponents::iterator i = canvases.begin();
			 i < canvases.end();
			 i++)
			[canvasComponents addObject:[NSValue valueWithPointer:*i]];
		
		storageComponents = [[NSMutableArray alloc] init];
		OEComponents &storages = deviceInfo->storages;
		for (OEComponents::iterator i = storages.begin();
			 i < storages.end();
			 i++)
		{
			[storageComponents addObject:[NSValue valueWithPointer:*i]];
			
			if ([document isStorageMounted:*i])
			{
				EmulationItem *storageItem;
				storageItem = [[EmulationItem alloc] initWithStorage:*i
														  deviceInfo:theDeviceInfo
														  inDocument:theDocument];
				[children addObject:storageItem];
				[storageItem release];
			}
		}
		
		settingsRefs = [[NSMutableArray alloc] init];
		settingsNames = [[NSMutableArray alloc] init];
		settingsLabels = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		for (EmulationSettings::iterator i = deviceInfo->settings.begin();
			 i != deviceInfo->settings.end();
			 i++)
		{
			[settingsRefs addObject:getNSString(i->ref)];
			[settingsNames addObject:getNSString(i->name)];
			[settingsLabels addObject:getNSString(i->label)];
			[settingsTypes addObject:getNSString(i->type)];
			[settingsOptions addObject:[getNSString(i->options)
										componentsSeparatedByString:@","]];
		}
	}
	
	return self;
}

- (id)initWithStorage:(void *)theComponent
		   deviceInfo:(void *)theDeviceInfo
		   inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		itemType = EMULATION_ITEM_STORAGE;
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		location = [getNSString(deviceInfo->location) retain];
		state = ([document isStorageWritable:theComponent] ?
				 @"Mounted Read-Only" : @"Mounted Read/Write");
		
		storageComponents = [[NSMutableArray alloc] initWithObjects:
							 [NSValue valueWithPointer:theComponent], 
							 nil];
		
		label = [[[self storagePath] lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[children release];
	
	[uid release];
	[image release];
	[label release];
	
	[location release];
	[state release];
	
	[canvasComponents release];
	
	[storageComponents release];
	
	[settingsRefs release];
	[settingsNames release];
	[settingsLabels release];
	[settingsTypes release];
	[settingsOptions release];
	
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

- (NSString *)uid
{
	return uid;
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
	return state;
}

- (BOOL)isGroup
{
	return (itemType == EMULATION_ITEM_GROUP);
}

- (BOOL)isRemovable
{
	return (itemType == EMULATION_ITEM_DEVICE) && [location length];
}



- (BOOL)isCanvas
{
	return ([canvasComponents count] != 0);
}



- (BOOL)isMountable
{
	return ([storageComponents count] != 0);
}

- (BOOL)isMounted
{
	return (itemType == EMULATION_ITEM_STORAGE);
}

- (NSString *)storagePath
{
	if (![storageComponents count])
		return @"";
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document getStoragePath:component];
}

- (NSString *)storageFormat
{
	if (![storageComponents count])
		return @"";
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document getStorageFormat:component];
}

- (NSString *)storageCapacity
{
	if (![storageComponents count])
		return @"";
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	return [document getStorageCapacity:component];
}

- (BOOL)mount:(NSString *)path
{
	for (int i = 0; i < [storageComponents count]; i++)
	{
		void *component = [[storageComponents objectAtIndex:i] pointerValue];
		
		if ([document mount:path inStorage:component])
			return YES;
	}
	
	if ([storageComponents count])
	{
		void *component = [[storageComponents objectAtIndex:0] pointerValue];
		
		[document unmountStorage:component];
		return [document mount:path inStorage:component];
	}
	return NO;
}

- (void)unmount
{
	void *component = [[storageComponents objectAtIndex:0] pointerValue];
	[document unmountStorage:component];
}

- (BOOL)isMountable:(NSString *)path
{
	for (int i = 0; i < [storageComponents count]; i++)
	{
		void *component = [[storageComponents objectAtIndex:i] pointerValue];
		
		if ([document isMountable:path inStorage:component])
			return YES;
	}
	return NO;
}



- (NSInteger)numberOfSettings
{
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
