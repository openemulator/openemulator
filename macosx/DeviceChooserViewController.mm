
/**
 * OpenEmulator
 * Mac OS X Device Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device chooser view.
 */

#import "DeviceChooserViewController.h"
#import "ChooserItem.h"
#import "Document.h"
#import "StringConversion.h"

#import "OEEDL.h"

@interface EDLInfo : NSObject
{
	NSString *path;
	OEEDLInfo edlInfo;
	OEConnectorsInfo connectorsInfo;
}

- (id)initWithPath:(NSString *)path;
- (NSString *)path;
- (OEEDLInfo *)edlInfo;
- (OEConnectorsInfo *)connectorsInfo;

@end

@implementation EDLInfo

- initWithPath:(NSString *)thePath;
{
	if (self = [super init])
	{
		path = [thePath copy];
		
		OEEDL edl;
		edl.open(getCString(thePath));
		edlInfo = edl.getEDLInfo();
		connectorsInfo = edl.getConnectorsInfo();
	}
	
	return self;
}

- (void)dealloc
{
	[path release];
	
	[super dealloc];
}

- (NSString *)path
{
	return path;
}

- (OEEDLInfo *)edlInfo
{
	return &edlInfo;
}

- (OEConnectorsInfo *)connectorsInfo
{
	return &connectorsInfo;
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
	
	// Load EDL infos from devices
	for (int i = 0; i < [devicesFilenames count]; i++)
	{
		NSString *path = [devicesFilenames objectAtIndex:i];
		path = [devicesPath stringByAppendingPathComponent:path];
		EDLInfo *info = [[EDLInfo alloc] initWithPath:path];
		
		if (info)
		{
			[info autorelease];
			[infos addObject:info];
		}
	}
}

- (void)updateForPorts:(NSArray *)ports
{
	[groups removeAllObjects];
	[groupNames removeAllObjects];
	
	// Find devices for current port configuration
	for (int i = 0; i < [infos count]; i++)
	{
		OEConnectorsInfo *connectorsInfo = [[infos objectAtIndex:i] connectorsInfo];
		NSMutableArray *availablePorts = [NSMutableArray arrayWithArray:ports];
		
		BOOL foundInlets = YES;
		for (OEPortsInfo::iterator connectorInfo = connectorsInfo->begin();
			 connectorInfo != connectorsInfo->end();
			 connectorInfo++)
		{
			NSString *connectorType = getNSString((*connectorInfo).type);
			BOOL foundInlet = NO;
			
			for (int j = 0; j < [availablePorts count]; j++)
			{
				NSString *portType = [availablePorts objectAtIndex:j];
				
				if ([portType compare:connectorType] == NSOrderedSame)
				{
					[availablePorts removeObjectAtIndex:j];
					
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
		
		// We found a device
		NSString *edlPath = [[infos objectAtIndex:i] path];
		OEEDLInfo *edlInfo = [[infos objectAtIndex:i] edlInfo];
		
		NSString *groupName = getNSString(edlInfo->type);
		NSString *label = getNSString(edlInfo->label);
		NSString *imageName = getNSString(edlInfo->image);
		NSString *description = getNSString(edlInfo->description);
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [resourcePath stringByAppendingPathComponent:imageName];
		ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
													  subtitle:description
													 imagePath:imagePath
														  data:edlPath];
		
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
/*	NSString *itemPath = [self selectedItemPath];
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
		[dict setObject:getNSString((*outlet)->image) forKey:@"image"];
		[dict setObject:getNSString((*outlet)->ref) forKey:@"ref"];
		
		[freeOutlets addObject:dict];
	}
	
	return freeOutlets;*/
	
	return nil;
}

@end
