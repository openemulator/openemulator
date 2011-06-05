
/**
 * OpenEmulator
 * Mac OS X Library Item
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a library item.
 */

#import "LibraryItem.h"
#import "StringConversion.h"
#import "OEEDL.h"

@implementation LibraryItem

- (id)initWithPath:(NSString *)thePath
{
    self = [super init];
    
	if (self)
	{
		path = [thePath copy];
        label = [[[path lastPathComponent] stringByDeletingPathExtension]
                 retain];
	}
	
	return self;
}

- (id)copyWithZone:(NSZone *)zone
{
    return [[LibraryItem alloc] initWithPath:path];
}

- (void)dealloc
{
	[path release];
    [label release];
    
    [image release];
    [type release];
	
	[super dealloc];
}

- (void)loadItem
{
    if (didLoad)
        return;
    
    didLoad = YES;
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    
    // Read EDL
	OEEDL edl;
    
    NSString *fullPath = [[resourcePath stringByAppendingPathComponent:@"library"]
                          stringByAppendingPathComponent:path];
	
	edl.open(getCPPString(fullPath));
	if (!edl.isOpen())
		return;
	
    NSString *imagePath = getNSString(edl.getHeaderInfo().image);
	OEConnectorsInfo connectorsInfo = edl.getFreeConnectorsInfo();
	
	edl.close();
    
    // Read image
    imagePath = [resourcePath stringByAppendingPathComponent:imagePath];
    
    image = [[NSImage alloc] initByReferencingFile:imagePath];
    
    // Read connector type
    if (connectorsInfo.size() >= 1)
        type = [getNSString(connectorsInfo.at(0).type) retain];
    
    // Read description
    description = [getNSString(edl.getHeaderInfo().description) retain];
}

- (NSString *)path
{
	return path;
}

- (NSString *)label
{
	return label;
}

- (NSImage *)image
{
    [self loadItem];
    
    return image;
}

- (NSString *)type
{
    [self loadItem];
    
	return type;
}

- (NSString *)description
{
    [self loadItem];
    
	return description;
}

@end
