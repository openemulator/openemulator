
/**
 * OpenEmulator
 * Mac OS X Template Chooser Item
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a template chooser item.
 */

#import "Quartz/Quartz.h"

#import "TemplateChooserItem.h"
#import "StringConversion.h"

#import "OEEDL.h"

@implementation TemplateChooserItem

- (id)initWithEDLPath:(NSString *)path
{
	if (self = [super init])
		edlPath = [path copy];
	
	return self;
}

- (void)dealloc
{
	[edlPath release];
	
	[label release];
	[description release];
	[edlPath release];
	
	[image release];
	
	[super dealloc];
}

- (void)update
{
	if (edlLoaded)
		return;
	
	OEEDL edl;
	edl.open(getCPPString(edlPath));
	if (edl.isOpen())
	{
		OEHeaderInfo headerInfo = edl.getHeaderInfo();
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		
		label = [[[edlPath lastPathComponent] stringByDeletingPathExtension]
				 retain];
		NSString *imagePath = [resourcePath stringByAppendingPathComponent:
							   getNSString(headerInfo.image)];
		image = [[NSImage alloc] initWithContentsOfFile:imagePath];
		description = [getNSString(headerInfo.description) retain];
	}
	
	edlLoaded = YES;
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
	
	return edlPath;
}

- (NSString *)description
{
	[self update];
	
	return description;
}

- (NSString *)edlPath
{
	return edlPath;
}

@end
