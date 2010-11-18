
/**
 * OpenEmulator
 * Mac OS X Chooser Item
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an item in a device or template chooser view.
 */

#import "ChooserItem.h"

@implementation ChooserItem

- (id)initWithTitle:(NSString *)theTitle
		   subtitle:(NSString *)theSubtitle
		  imagePath:(NSString *)theImagePath
			edlPath:(NSString *)theEDLPath
			   data:(void *)theData
{
	if (self = [super init])
	{
		if (theTitle)
			title = [theTitle copy];
		
		if (theSubtitle)
			subtitle = [theSubtitle copy];
		
		if (theImagePath)
			imagePath = [theImagePath copy];
		
		if (theEDLPath)
			edlPath = [theEDLPath copy];
		
		data = theData;
	}
	
	return self;
}

- (void)dealloc
{
    [title release];
    [subtitle release];
    [imagePath release];
    [edlPath release];
	
    [super dealloc];
}

- (NSString *)imageRepresentationType
{
	return IKImageBrowserPathRepresentationType;
}

- (id)imageRepresentation
{
	return imagePath;
}

- (NSString *)imageTitle
{
	return title;
}

- (NSString *)imageSubtitle
{
	return subtitle;
}

- (NSString *)imageUID
{
    return [[edlPath retain] autorelease];
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
