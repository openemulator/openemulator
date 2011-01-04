
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
			
			NSString *groupName = getNSString(deviceInfo.group);
			EmulationItem *groupItem = [self childWithUid:groupName];
			if (!groupItem)
			{
				groupItem = [[EmulationItem alloc] initWithLabel:groupName];
				[children addObject:groupItem];
				[groupItem release];
			}
			
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
		uid = [theLabel copy];
		label = [[NSLocalizedString(theLabel, @"Emulation Item Label.")
				  uppercaseString] retain];
		
		children = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (id)initWithDeviceInfo:(void *)theDeviceInfo
			  inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		EmulationDeviceInfo *deviceInfo = (EmulationDeviceInfo *)theDeviceInfo;
		
		document = theDocument;
		
		uid = [getNSString(deviceInfo->id) retain];
		label = [getNSString(deviceInfo->label) retain];
		image = [[self getImage:getNSString(deviceInfo->image)] retain];
		location = [getNSString(deviceInfo->location) retain];
		state = [getNSString(deviceInfo->state) retain];
		canvas = (deviceInfo->canvases.size() != 0);
		storage = (deviceInfo->storages.size() != 0);
		
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
		
		children = [[NSMutableArray alloc] init];
		
		for (OEComponents::iterator i = deviceInfo->storages.begin();
			 i != deviceInfo->storages.end();
			 i++)
		{
			NSString *path = [[document pathOfImageInStorage:*i]
							  lastPathComponent];
			if ([path length])
			{
				EmulationItem *diskImageItem;
				diskImageItem = [[EmulationItem alloc] initWithDiskImageAtPath:path
																	   storage:*i
																	  location:location
																	inDocument:theDocument];
				[children addObject:diskImageItem];
				[diskImageItem release];
			}
			
			storageComponent = *i;
		}
	}
	
	return self;
}

- (id)initWithDiskImageAtPath:(NSString *)thePath
					  storage:(void *)theComponent
					 location:(NSString *)theLocation
				   inDocument:(Document *)theDocument
{
	if (self = [super init])
	{
		document = theDocument;
		
		uid = @"";
		label = [[thePath lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
		location = [theLocation copy];
		state = @"Mounted Read/Write";
		
		mounted = YES;
		storage = NO;
		diskImagePath = [thePath copy];
		storageComponent = theComponent;
		
		children = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[uid release];
	[image release];
	[label release];
	[location release];
	[state release];
	
	[diskImagePath release];
	
	[settingsRefs release];
	[settingsNames release];
	[settingsLabels release];
	[settingsTypes release];
	[settingsOptions release];
	
	[children release];
	
	[super dealloc];
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

- (BOOL)canvas
{
	return canvas;
}

- (BOOL)storage
{
	return storage;
}

- (BOOL)mounted
{
	return mounted;
}

- (BOOL)locked
{
	return NO;
}

- (NSString *)storagePath
{
	return diskImagePath;
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

@end
