
/**
 * AppleIIGo Mac OS X
 * InfoWindow Controller
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import "InfoWindowController.h"

typedef struct tabType
{
	NSString * identifier;
	int32_t tag;
} tab;

typedef enum
	{
		TAB_TAG_INFO = 0,
		TAB_TAG_EXPANSION = 1,
		TAB_TAG_VIDEO = 2,
		TAB_TAG_AUDIO = 3,
		TAB_TAG_NONE = 4,
	} tabEnum;

tab tabData[] =
{
@"Information", TAB_TAG_INFO,
@"Expansions", TAB_TAG_EXPANSION,
@"Video", TAB_TAG_VIDEO,
@"Audio", TAB_TAG_AUDIO,
nil, 0,
};

@implementation InfoWindowController

- (id) init
{
	return [super initWithWindowNibName:@"InfoWindow"];
}

- (void) awakeFromNib
{
	NSPanel * window = (NSPanel *)[self window];
	tab * tabP;
	
	// Initialize frame
	[window setFrameAutosaveName:@"InfoWindow"];
	[window setFrameUsingName:@"InfoWindow"];
    NSRect windowRect = [window frame];
	float infoWindowHeight = [[NSUserDefaults standardUserDefaults]
								floatForKey:@"InfoWindowHeight"];
	windowRect.origin.y += (infoWindowHeight - windowRect.size.height);
	[window setFrame:windowRect display:NO];
	[window setBecomesKeyOnlyIfNeeded:YES];
	
	fCurrentTabTag = TAB_TAG_NONE;
	NSString * identifier = [[NSUserDefaults standardUserDefaults]
							 stringForKey:@"InfoWindowView"];
	
	// Select cell
	int tag = TAB_TAG_INFO;
	for (tabP = tabData; tabP->identifier; tabP++)
		if ([identifier isEqualToString:tabP->identifier])
			tag = tabP->tag;
	[fTabMatrix selectCellWithTag:tag];
	[self setTab:nil];
}

- (void) setTab:(id) sender
{
	int oldTabTag = fCurrentTabTag;
	fCurrentTabTag = [fTabMatrix selectedTag];
	if (fCurrentTabTag == oldTabTag)
		return;
	
	// Remove old view
	float oldViewHeight = 0;
	if (oldTabTag != TAB_TAG_NONE)
	{
		NSView * oldView = [self tabViewForTag:oldTabTag];
		oldViewHeight = [oldView frame].size.height;
		
		[oldView setHidden:YES];
		[oldView removeFromSuperview];
	}
	
	// Update view
	[self updateInfoStats];
	
	NSView * view = [self tabViewForTag:fCurrentTabTag];
	
	// Find new tab item
	tab * tabP;
	NSString * identifier = nil;
	for (tabP = tabData; tabP->identifier; tabP++)
		if (fCurrentTabTag == tabP->tag)
			identifier = tabP->identifier;
	if (!identifier)
		return;
	
	// Set new tab item
	NSWindow * window = [self window];
	[window setTitle:NSLocalizedString(identifier, "Inspector -> title")];
	
	NSRect windowRect = [window frame];
	NSRect viewRect = [view frame];
	float viewHeightDifference = (viewRect.size.height - oldViewHeight) *
	[window userSpaceScaleFactor];
	windowRect.origin.y -= viewHeightDifference;
	windowRect.size.height += viewHeightDifference;
	
	[window setFrame:windowRect display:YES animate:oldTabTag != TAB_TAG_NONE];
	[[window contentView] addSubview:view];
	[view setHidden:NO];
	
	float infoWindowHeight = windowRect.size.height;
	[[NSUserDefaults standardUserDefaults] setObject:identifier
											  forKey:@"InfoWindowView"];
	[[NSUserDefaults standardUserDefaults] setFloat:infoWindowHeight
											 forKey:@"InfoWindowHeight"];
}

- (NSView *) tabViewForTag:(int) tag
{
	switch (tag)
	{
		case TAB_TAG_INFO:
			return fInfoView;
		case TAB_TAG_EXPANSION:
			return fExpansionView;
		case TAB_TAG_VIDEO:
			return fVideoView;
		case TAB_TAG_AUDIO:
			return fAudioView;
		default:
			return nil;
	}
}

- (void) updateInfoStats
{
	switch ([fTabMatrix selectedTag])
	{
		case TAB_TAG_INFO:
			[self updateInfoGeneral];
			break;
		case TAB_TAG_EXPANSION:
			[self updateInfoGeneral];
			break;
		case TAB_TAG_VIDEO:
			[self updateInfoGeneral];
			break;
		case TAB_TAG_AUDIO:
			[self updateInfoGeneral];
			break;
	}
}

- (void) updateInfoGeneral
{   
}

@end
