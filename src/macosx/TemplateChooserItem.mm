
/**
 * OpenEmulator
 * Mac OS X Template Chooser Item
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser item
 */

#import "Quartz/Quartz.h"

#import "TemplateChooserItem.h"

#import "NSStringAdditions.h"

#import "OEDocument.h"

@implementation TemplateChooserItem

- (id)initWithOEDocumentPath:(NSString *)thePath
{
    self = [super init];
    
    if (self)
        path = [thePath copy];
    
    return self;
}

- (void)dealloc
{
    [path release];
    
    [label release];
    [image release];
    [description release];
    
    [super dealloc];
}

- (void)update
{
    if (loaded)
        return;
    
    OEDocument oeDocument;
    oeDocument.open([path cppString]);
    if (oeDocument.isOpen())
    {
        OEHeaderInfo headerInfo = oeDocument.getHeaderInfo();
        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        
        label = [[[path lastPathComponent] stringByDeletingPathExtension]
                 retain];
        NSString *imagePath = [resourcePath stringByAppendingPathComponent:
                               [NSString stringWithCPPString:headerInfo.image]];
        image = [[NSImage alloc] initWithContentsOfFile:imagePath];
        description = [[NSString stringWithCPPString:headerInfo.description] retain];
    }
    
    loaded = YES;
}

- (NSString *)imageRepresentationType
{
    return IKImageBrowserNSImageRepresentationType;
}

- (id)imageRepresentation
{
    [self update];
    
    return image;
}

- (NSString *)imageTitle
{
    [self update];
    
    return label;
}

- (NSString *)imageSubtitle
{
    return @"";
}

- (NSString *)imageUID
{
    [self update];
    
    return path;
}

- (NSString *)description
{
    [self update];
    
    return description;
}

- (NSString *)path
{
    return path;
}

@end
