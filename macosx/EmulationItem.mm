
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
				groupItem = [[EmulationItem alloc] initWithLabel:groupName];
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

- (id)initWithLabel:(NSString *)theLabel
{
	if (self = [super init])
	{
		children = [[NSMutableArray alloc] init];
		
		uid = [theLabel copy];
		label = [[NSLocalizedString(theLabel, @"Emulation Item Label.")
				  uppercaseString] retain];
	}
	
	return self;
}

- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		imagePath = [[resourcePath stringByAppendingPathComponent:
					  getNSString(deviceInfo->image)] retain];
		
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		isRemovable = ([location length] != 0);
		
		isCanvas = (deviceInfo->canvases.size() != 0);
		
		isStorage = (deviceInfo->storages.size() != 0);
		
		settingsRefs = [[NSMutableArray alloc] init];
		settingsNames = [[NSMutableArray alloc] init];
		settingsLabels = [[NSMutableArray alloc] init];
		settingsTypes = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		
		for (EmulationSettings::iterator i = deviceInfo->settings.begin();
			 i != deviceInfo->settings.end();
			 i++)
		{
			NSString *settingRef = getNSString(i->ref);
			NSString *settingName = getNSString(i->name);
			NSString *settingLabel = getNSString(i->label);
			NSString *settingType = getNSString(i->type);
			NSArray *settingOptions = [getNSString(i->options)
									   componentsSeparatedByString:@","];
			
			[settingsRefs addObject:settingRef];
			[settingsNames addObject:settingName];
			[settingsLabels addObject:settingLabel];
			[settingsTypes addObject:settingType];
			[settingsOptions addObject:settingOptions];
		}
		
		for (OEComponents::iterator i = deviceInfo->storages.begin();
			 i != deviceInfo->storages.end();
			 i++)
		{
			if ([document storageMounted:*i])
			{
				EmulationItem *storageItem;
				storageItem = [[EmulationItem alloc] initWithStorage:*i
														  deviceInfo:theDeviceInfo
														  inDocument:theDocument];
				[children addObject:storageItem];
				[storageItem release];
			}
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
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		location = [getNSString(deviceInfo->location) retain];
		state = ([document storageWritable:theComponent] ?
				 @"Mounted Read-Only" : @"Mounted Read/Write");
		
		storageComponent = theComponent;
		
		label = [[[self storagePath] lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[children release];
	
	[uid release];
	[imagePath release];
	[label release];
	
	[location release];
	[state release];
	
	[settingsRefs release];
	[settingsNames release];
	[settingsLabels release];
	[settingsTypes release];
	[settingsOptions release];
	
	[image release];
	
	[super dealloc];
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

- (NSImage *)getImage:(NSString *)path
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	path = [resourcePath stringByAppendingPathComponent:path];
	
	return [[[NSImage alloc] initByReferencingFile:path] autorelease];
}

- (NSString *)uid
{
	return uid;
}

- (NSImage *)image
{
	if (!image)
		image = [[NSImage alloc] initByReferencingFile:imagePath];
	
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

- (BOOL)isRemovable
{
	return isRemovable;
}

- (BOOL)isCanvas
{
	return isCanvas;
}

- (BOOL)isStorage
{
	return isStorage;
}

- (BOOL)isStorageMounted
{
	return (storageComponent != nil);
}

- (NSString *)storagePath
{
	return [document storagePath:storageComponent];
}

- (NSString *)storageFormat
{
	return [document storageFormat:storageComponent];
}

- (NSString *)storageCapacity
{
	return [document storageCapacity:storageComponent];
}

- (BOOL)mount:(NSString *)path
{
	return [document mount:path inStorage:storageComponent];
}

- (void)unmount
{
	[document unmountStorage:storageComponent];
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
