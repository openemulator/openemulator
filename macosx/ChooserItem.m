
/**
 * OpenEmulator
 * Mac OS X Chooser Item
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an item in a device or template chooser view.
 */

#import "ChooserItem.h"

@implementation ChooserItem

- (id)initWithLabel:(NSString *)theLabel
		  imagePath:(NSString *)theImagePath
		description:(NSString *)theDescription
			edlPath:(NSString *)theEDLPath
			   data:(void *)theData
{
	if (self = [super init])
	{
		if (theLabel)
			label = [theLabel copy];
		if (theImagePath)
			imagePath = [theImagePath copy];
		
		if (theDescription)
			description = [theDescription copy];
		if (theEDLPath)
			edlPath = [theEDLPath copy];
		data = theData;
	}
	
	return self;
}

- (void)dealloc
{
	[label release];
	[imagePath release];
	
	[description release];
	[edlPath release];
	
	[image release];
	
	[super dealloc];
}

- (NSString *)imageRepresentationType
{
	return IKImageBrowserNSImageRepresentationType;
}

- (id)imageRepresentation
{
	if (!image)
		image = [[NSImage alloc] initWithContentsOfFile:imagePath];	
	
	return image;
}

- (NSString *)imageTitle
{
	return label;
}

- (NSString *)imageSubtitle
{
	return @"";
}

- (NSString *)imageUID
{
	return edlPath;
}

- (NSString *)description
{
	return description;
}

- (NSString *)edlPath
{
	return edlPath;
}

- (void *)data
{
	return data;
}

@end
