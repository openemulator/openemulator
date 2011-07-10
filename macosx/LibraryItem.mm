
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
#import "OEDocument.h"

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
    
    // Read OE document
	OEDocument oeDocument;
    
    NSString *fullPath = [[resourcePath stringByAppendingPathComponent:@"library"]
                          stringByAppendingPathComponent:path];
	
	oeDocument.open(getCPPString(fullPath));
	if (!oeDocument.isOpen())
		return;
	
    NSString *imagePath = getNSString(oeDocument.getHeaderInfo().image);
	OEConnectorInfos connectorInfos = oeDocument.getFreeConnectorInfos();
	
	oeDocument.close();
    
    // Read image
    imagePath = [resourcePath stringByAppendingPathComponent:imagePath];
    
    image = [[NSImage alloc] initByReferencingFile:imagePath];
    
    // Read connector type
    if (connectorInfos.size() == 1)
    {
        OEConnectorInfos::iterator i = connectorInfos.begin();
        type = [getNSString(i->type) retain];
    }
    
    // Read description
    description = [getNSString(oeDocument.getHeaderInfo().description) retain];
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
