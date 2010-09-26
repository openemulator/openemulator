
/**
 * OpenEmulator
 * Mac OS X Device Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device chooser view.
 */

#import "DeviceChooserViewController.h"

#import "OEInfo.h"

#import "ChooserItem.h"
#import "Document.h"

#import "StringConversion.h"

@interface InfoWrapper : NSObject
{
	OEInfo *info;
	NSString *path;
}

- (id)initWithPath:(NSString *)path;
- (OEInfo *)info;
- (NSString *)path;

@end

@implementation InfoWrapper

- initWithPath:(NSString *)thePath;
{
	if (self = [super init])
	{
		info = new OEInfo(string([thePath UTF8String]));
		path = [thePath copy];
	}
	
	return self;
}

- (void)dealloc
{
	delete info;
	[path release];
	
	[super dealloc];
}

- (OEInfo *)info
{
	return info;
}

- (NSString *)path
{
	return path;
}

@end

@implementation DeviceChooserViewController

- (id)init
{
	self = [super init];
	
	if (self)
		infos = [[NSMutableArray alloc] init];
	
	return self;
}

- (void)dealloc
{
	[infos release];
	
	[super dealloc];
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *devicesPath = [resourcePath stringByAppendingPathComponent:@"devices"];
	NSArray *devicesFilenames = [[NSFileManager defaultManager]
								 contentsOfDirectoryAtPath:devicesPath
								 error:nil];
	
	for (int i = 0; i < [devicesFilenames count]; i++)
	{
		NSString *path = [devicesFilenames objectAtIndex:i];
		path = [devicesPath stringByAppendingPathComponent:path];
		InfoWrapper *info = [[InfoWrapper alloc] initWithPath:path];
		
		if (info)
		{
			[info autorelease];
			[infos addObject:info];
		}
	}
}

- (void)updateForDeviceType:(NSString *)deviceType
			 withFreeInlets:(NSArray *)freeInlets
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *imagesPath = [resourcePath stringByAppendingPathComponent:@"images"];
	
	[groups removeAllObjects];
	[groupNames removeAllObjects];
	
	for (int i = 0; i < [infos count]; i++)
	{
		// Check if the required inlets are available
		OEInfo *info = [[infos objectAtIndex:i] info];
		NSString *infoPath = [[infos objectAtIndex:i] path];
		
		NSMutableArray *inlets = [NSMutableArray arrayWithArray:freeInlets];
		OEPorts *outlets = info->getOutlets();
		
		BOOL foundInlets = YES;
		
		for (OEPorts::iterator outlet = outlets->begin();
			 outlet != outlets->end();
			 outlet++)
		{
			if ((*outlet)->connection)
				continue;
			
			NSString *outletType = getNSString((*outlet)->type);
			BOOL foundInlet = NO;
			
			for (int j = 0; j < [inlets count]; j++)
			{
				NSMutableDictionary *dict = [inlets objectAtIndex:j];
				NSString *inletType = [dict objectForKey:@"type"];
				
				if ([inletType compare:outletType] == NSOrderedSame)
				{
					[inlets removeObjectAtIndex:j];
					
					foundInlet = YES;
					
					break;
				}
			}
			
			if (!foundInlet)
			{
				foundInlets = NO;
				break;
			}
		}
		
		if (!foundInlets)
			continue;
		
		// Check if there are devices of requested type
		OEDevices *devices = info->getDevices();
		bool foundType = NO;
		
		for (OEDevices::iterator device = devices->begin();
			 device != devices->end();
			 device++)
		{
			NSString *thisDeviceType = getNSString((*device)->type);
			if ([deviceType compare:thisDeviceType] == NSOrderedSame)
				foundType = YES;
		}
		
		if (!foundType)
			continue;
		
		// Add device
		NSString *groupName = getNSString(info->getType());
		NSString *label = getNSString(info->getLabel());
		NSString *imageName = getNSString(info->getImageSrc());
		NSString *description = getNSString(info->getDescription());
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		
		NSString *imagePath = [imagesPath stringByAppendingPathComponent:imageName];
		ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
													  subtitle:description
													 imagePath:imagePath
														  data:infoPath];
		
		if (!item)
			continue;
		
		[item autorelease];
		[[groups objectForKey:groupName] addObject:item];
	}
	
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
	
	[self selectItemWithPath:nil];
}

- (NSMutableArray *)selectedItemOutlets
{
	// Find selected device
	NSString *itemPath = [self selectedItemPath];
	if (!itemPath)
		return NULL;
	
	OEInfo *info = NULL;
	for (int i = 0; i < [infos count]; i++)
	{
		NSString *path = [[infos objectAtIndex:i] path];
		if ([path compare:itemPath] == NSOrderedSame)
		{
			info = [[infos objectAtIndex:i] info];
			break;
		}
	}
	
	if (!info)
		return NULL;
	
	// Get unconnected outlets
	NSMutableArray *freeOutlets = [[NSMutableArray alloc] init];
	if (!freeOutlets)
		return NULL;
	[freeOutlets autorelease];
	
	OEPorts *outlets = info->getOutlets();
	for (OEPorts::iterator outlet = outlets->begin();
		 outlet != outlets->end();
		 outlet++)
	{
		if ((*outlet)->connection)
			continue;
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:getNSString((*outlet)->type) forKey:@"type"];
		[dict setObject:getNSString((*outlet)->label) forKey:@"label"];
		[dict setObject:getNSString((*outlet)->imageSrc) forKey:@"image"];
		[dict setObject:getNSString((*outlet)->ref) forKey:@"ref"];
		
		[freeOutlets addObject:dict];
	}
	
	return freeOutlets;
}

@end
