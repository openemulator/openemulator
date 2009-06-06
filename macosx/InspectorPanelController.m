
/**
 * OpenEmulator
 * Mac OS X InspectorPanel Controller
 * (C) 2008-2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import "InspectorPanelController.h"
#import "Document.h"

NSString *itemIdentifiers[] = 
{
	@"Emulation",
	@"Expansions",
	@"Disk Drives",
	@"Ports",
	@"Video",
	@"Audio",
};

@implementation InspectorPanelController

- (id)init
{
	self = [super initWithWindowNibName:@"InspectorPanel"];
	
    if (self) {
        fDefaults = [NSUserDefaults standardUserDefaults];

		NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
		[nc addObserver:self
			   selector:@selector(toggleInspectorPanelNotification:)
				   name:@"toggleInspectorPanelNotification"
				 object:nil];
/*		[nc addObserver:self
			   selector:@selector(inspectorPanelDidResignKey:)
				   name:NSWindowDidResignKeyNotification
				 object:[self window]];
*/		
		oldTabTag = -1;
    }

    return self;
}

- (void)awakeFromNib
{
	NSPanel *window = (NSPanel *)[self window];
	[window setBecomesKeyOnlyIfNeeded:YES];
	
	int tabTag = [fDefaults integerForKey:@"InspectorPanelView"];
	[fTabMatrix selectCellWithTag:tabTag];
	[self setView:tabTag isInit:TRUE];
}

- (void)windowDidLoad
{
	if ([fDefaults boolForKey:@"InspectorPanelIsVisible"])
		[self toggleInspectorPanel:nil];
	
/*    [self activeDocumentChanged];
    [NSApp addObserver:self
			forKeyPath:@"mainWindow.windowController.document"
			   options:0
			   context:[InspectorPanelController class]];
*/	
    [super windowDidLoad];
}

- (void)toggleInspectorPanelNotification:(NSNotification *)notification
{
	[self toggleInspectorPanel:[notification object]];
}

- (void)toggleInspectorPanel:(id)sender
{
    NSWindow *window = [self window];
    if ([window isVisible])
		[window close];
	else
		[window orderFront:nil];
    [fDefaults setBool:[window isVisible] forKey:@"InspectorPanelIsVisible"];
}

- (BOOL)validateUserInterfaceItem:(id)item
{
    if ([item action] == @selector(toggleInspectorPanel:))
	{  
		NSString *menuTitle = nil;

		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Inspector",
										  @"Title for menu item to show the Inspector panel"
										  "(should be the same as the initial menu item in the nib).");
		else
			menuTitle = NSLocalizedString(@"Hide Inspector",
										  @"Title for menu item to hide the Inspector panel.");
		
		[item setTitleWithMnemonic:menuTitle];
    }
	
    return YES;
}

- (void)activeDocumentChanged
{
    id mainDocument = [[[NSApp mainWindow] windowController] document];
    if (mainDocument == inspectedDocument)
		return;
	
	if (inspectedDocument)
		[documentController commitEditing];
	
	if (mainDocument && [mainDocument isKindOfClass:[Document class]])
		inspectedDocument = mainDocument;
	else
		inspectedDocument = nil;
}

- (void)inspectorPanelDidResignKey:(NSNotification *)notification
{
    [documentController commitEditing];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object
						change:(NSDictionary *)change
					   context:(void *)context
{
    if (context == [InspectorPanelController class])
		[self activeDocumentChanged];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSView *)getView:(int)tabTag
{
	switch (tabTag)
	{
		case 0:
			return fEmulationView;
		case 1:
			return fExpansionsView;
		case 2:
			return fDiskDrivesView;
		case 3:
			return fPortsView;
		case 4:
			return fVideoView;
		case 5:
			return fAudioView;
	}
	
	return fEmulationView;
}

- (void)selectView:(id)sender
{
	[self setView:[fTabMatrix selectedTag] isInit:FALSE];
}

- (void)setView:(int)tabTag isInit:(bool)isInit
{
	if (tabTag == oldTabTag)
		 return;
	
	float oldHeight = 0;
	if (oldTabTag >= 0)
	{
		NSView *oldView = [self getView:oldTabTag];
		oldHeight = [oldView frame].size.height;
		
		[oldView setHidden:YES];
		[oldView removeFromSuperview];
	}
	
	NSWindow *window = [self window];
	NSView *view = [self getView:tabTag];
	
	NSRect windowRect = [window frame];
	
	float difference = ([view frame].size.height - oldHeight) *
		[window userSpaceScaleFactor];
	if (!isInit)
		windowRect.origin.y -= difference;
	windowRect.size.height += difference;
	
    [window setFrame:windowRect display:YES animate:YES];
	[[window contentView] addSubview:view];
    [view setHidden:NO];
    
	[window setTitle:NSLocalizedString(itemIdentifiers[tabTag], "Inspector Panel view")];
	
    [fDefaults setInteger:tabTag forKey:@"InspectorPanelView"];
	oldTabTag = tabTag;
}

@end
