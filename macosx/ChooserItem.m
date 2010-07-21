
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
			   data:(NSString *)theData
{
	if (self = [super init])
	{
		if (theTitle)
			title = [theTitle copy];
		if (theSubtitle)
			subtitle = [theSubtitle copy];
		if (theImagePath)
			imagePath = [theImagePath copy];
		if (theData)
			data = [theData copy];
	}
	
	return self;
}

- (void)dealloc
{
    [title release];
    [subtitle release];
    [imagePath release];
    [data release];
	
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
    return [[data retain] autorelease];
}

- (NSString *)data
{
	return [[data retain] autorelease];
}

@end
