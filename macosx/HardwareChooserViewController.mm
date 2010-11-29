
/**
 * OpenEmulator
 * Mac OS X Hardware Chooser View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a hardware chooser view.
 */

#import "HardwareChooserViewController.h"
#import "ChooserItem.h"
#import "Document.h"
#import "StringConversion.h"

#import "OEEDL.h"

@interface EDLInfo : NSObject
{
	NSString *path;
	OEHeaderInfo headerInfo;
	OEConnectorsInfo freeConnectorsInfo;
}

- (id)initWithPath:(NSString *)path;
- (NSString *)path;
- (OEHeaderInfo *)headerInfo;
- (OEConnectorsInfo *)freeConnectorsInfo;

@end

@implementation EDLInfo

- initWithPath:(NSString *)thePath;
{
	if (self = [super init])
	{
		path = [thePath copy];
		
		OEEDL edl;
		edl.open(getCString(thePath));
		
		headerInfo = edl.getHeaderInfo();
		freeConnectorsInfo = edl.getFreeConnectorsInfo();
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

- (OEHeaderInfo *)headerInfo
{
	return &headerInfo;
}

- (OEConnectorsInfo *)freeConnectorsInfo
{
	return &freeConnectorsInfo;
}

@end

@implementation HardwareChooserViewController

- (id)init
{
	self = [super init];
	
	if (self)
		edlInfos = [[NSMutableArray alloc] init];
	
	return self;
}

- (void)dealloc
{
	[edlInfos release];
	
	[super dealloc];
}

- (void)awakeFromNib
{
	[super awakeFromNib];
	
	NSString *devicesPath = [[[NSBundle mainBundle] resourcePath]
							 stringByAppendingPathComponent:@"devices"];
	NSArray *devicesFilenames = [[NSFileManager defaultManager]
								 contentsOfDirectoryAtPath:devicesPath
								 error:nil];
	
	// Load info from devices
	for (int i = 0; i < [devicesFilenames count]; i++)
	{
		NSString *path = [devicesFilenames objectAtIndex:i];
		path = [devicesPath stringByAppendingPathComponent:path];
		EDLInfo *edlInfo = [[EDLInfo alloc] initWithPath:path];
		
		if (edlInfo)
		{
			[edlInfo autorelease];
			[edlInfos addObject:edlInfo];
		}
	}
}

- (void)updateForPorts:(NSArray *)ports
{
	[groups removeAllObjects];
	[groupNames removeAllObjects];
	
	// Find devices matching available ports
	for (int i = 0; i < [edlInfos count]; i++)
	{
		NSString *edlPath = [[edlInfos objectAtIndex:i] path];
		
		OEConnectorsInfo *connectors = [[edlInfos objectAtIndex:i] freeConnectorsInfo];
		NSMutableArray *portsLeft = [NSMutableArray arrayWithArray:ports];
		bool isMatch = YES;
		for (OEPortsInfo::iterator connector = connectors->begin();
			 connector != connectors->end();
			 connector++)
		{
			NSString *connectorType = getNSString(connector->type);
			
			int index = [portsLeft indexOfObject:connectorType];
			if (index != NSNotFound)
				[portsLeft removeObjectAtIndex:index];
			else
			{
				isMatch = NO;
				break;
			}
		}
		if (!isMatch)
			continue;
		
		// Add a device and its group if necessary
		OEHeaderInfo *headerInfo = [[edlInfos objectAtIndex:i] headerInfo];
		NSString *groupName = getNSString(headerInfo->type);
		NSString *label = getNSString(headerInfo->label);
		NSString *imageName = getNSString(headerInfo->image);
		NSString *description = getNSString(headerInfo->description);
		
		if (![groups objectForKey:groupName])
		{
			NSMutableArray *group = [[[NSMutableArray alloc] init] autorelease];
			[groups setObject:group forKey:groupName];
		}
		
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [resourcePath stringByAppendingPathComponent:imageName];
		EDLInfo *edlInfo = [edlInfos objectAtIndex:i];
		ChooserItem *item = [[ChooserItem alloc] initWithTitle:label
													  subtitle:description
													 imagePath:imagePath
													   edlPath:edlPath
														  data:edlInfo];
		
		if (item)
		{
			[item autorelease];
			[[groups objectForKey:groupName] addObject:item];
		}
	}
	
	[groupNames addObjectsFromArray:[[groups allKeys]
									 sortedArrayUsingSelector:@selector(compare:)]];
	
	[self selectItemWithPath:nil];
}

- (NSMutableArray *)selectedItemConnectors
{
	NSString *itemPath = [self selectedItemPath];
	OEConnectorsInfo *connectors = nil;
	for (int i = 0; i < [edlInfos count]; i++)
	{
		NSString *path = [[edlInfos objectAtIndex:i] path];
		if ([path compare:itemPath] == NSOrderedSame)
		{
			connectors = [[edlInfos objectAtIndex:i] freeConnectorsInfo];
			break;
		}
	}
	if (!connectors)
		return nil;
	
	// Get connectors
	NSMutableArray *ports = [[NSMutableArray alloc] init];
	if (!ports)
		return nil;
	[ports autorelease];
	
	for (OEConnectorsInfo::iterator connector = connectors->begin();
		 connector != connectors->end();
		 connector++)
	{
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:getNSString(connector->id) forKey:@"id"];
		[dict setObject:getNSString(connector->type) forKey:@"type"];
		[dict setObject:getNSString(connector->label) forKey:@"label"];
		[dict setObject:getNSString(connector->image) forKey:@"image"];
		
		[ports addObject:dict];
	}
	
	return ports;
}

@end
