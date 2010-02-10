
/**
 * OpenEmulator
 * Mac OS X Inspector Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the inspector window.
 */

#import "InspectorController.h"
#import "Document.h"

NSString *itemIdentifiers[] = 
{
	@"Emulation",
	@"Expansions",
	@"Storage",
	@"Peripherals",
	@"Audio",
};

@implementation InspectorController

- (id)init
{
	if (self = [super initWithWindowNibName:@"Inspector"])
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(toggleInspector:)
													 name:@"toggleInspector"
												   object:nil];
		
		selectedViewIndex = -1;
	}
	
	return self;
}

- (void) windowDidLoad
{
	[self setWindowFrameAutosaveName:@"Inspector"];
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	int viewIndex = [userDefaults integerForKey:@"OEInspectorSelectedViewIndex"];
 	[fTabMatrix selectCellWithTag:viewIndex];
	[self setView:viewIndex];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(inspectorDidResignKey:)
												 name:NSWindowDidResignKeyNotification
											   object:[self window]];
	[self activeDocumentDidChange];
    [NSApp addObserver:self
			forKeyPath:@"mainWindow.windowController.document"
			   options:0
			   context:[InspectorController class]];
}

- (void) observeValueForKeyPath:(NSString *) keyPath
					   ofObject:(id) object
						 change:(NSDictionary *) change
						context:(void *) context
{
    if (context == [InspectorController class])
		[self activeDocumentDidChange];
	else
		[super observeValueForKeyPath:keyPath
							 ofObject:object
							   change:change
							  context:context];
}

- (void) activeDocumentDidChange
{
    id activeDocument = [[[NSApp mainWindow] windowController] document];
    if (activeDocument != inspectedDocument)
	{
		if (inspectedDocument)
			[fDocumentObjectController commitEditing];
		
		self.inspectedDocument = (activeDocument && [activeDocument
													 isKindOfClass:[Document class]
													 ]) ? activeDocument : nil;   
    }
}

- (void) inspectorDidResignKey:(NSNotification *) notification
{
    [fDocumentObjectController commitEditing];
}

- (id) inspectedDocument
{
	return inspectedDocument;
}

- (void) setInspectedDocument:(id) value
{
	inspectedDocument = value;
}

- (void) toggleInspector:(id) sender
{
    NSWindow *window = [self window];
    if ([window isVisible])
		[window orderOut:sender];
	else
		[window orderFront:sender];
}

- (BOOL) validateUserInterfaceItem:(id) item
{
    if ([item action] == @selector(toggleInspector:))
	{  
		NSString *menuTitle;
		if (![[self window] isVisible])
			menuTitle = NSLocalizedString(@"Show Inspector",
										  @"Title for menu item to show the Inspector "
										  "(should be the same as the initial menu item in "
										  "the nib).");
		else
			menuTitle = NSLocalizedString(@"Hide Inspector",
										  @"Title for menu item to hide the Inspector.");
		[item setTitleWithMnemonic:menuTitle];
    }
	
    return YES;
}

- (NSView *) getView:(int) viewIndex
{
	switch (viewIndex)
	{
		case 0:
			return fEmulationView;
		case 1:
			return fExpansionsView;
		case 2:
			return fStorageView;
		case 3:
			return fPeripheralsView;
		case 4:
			return fAudioView;
	}
	
	return fEmulationView;
}

- (void) selectView:(id) sender
{
	[self setView:[fTabMatrix selectedTag]];
}

- (void) setView:(int) viewIndex
{
	if (selectedViewIndex == viewIndex)
		return;
	
	float selectedHeight = 0;
	if (selectedViewIndex >= 0)
	{
		NSView *selectedView = [self getView:selectedViewIndex];
		selectedHeight = [selectedView frame].size.height;
		
		[selectedView setHidden:YES];
		[selectedView removeFromSuperview];
	}
	
	NSWindow *window = [self window];
	NSRect frame = [window frame];
	
	NSView *view = [self getView:viewIndex];
	float difference = ([view frame].size.height - selectedHeight) *
	[window userSpaceScaleFactor];
	if (selectedViewIndex >= 0)
		frame.origin.y -= difference;
	frame.size.height += difference;
	
    [window setFrame:frame display:YES animate:YES];
	[[window contentView] addSubview:view];
    [view setHidden:NO];
    
	[window setTitle:NSLocalizedString(itemIdentifiers[viewIndex], "")];
	
	selectedViewIndex = viewIndex;
	
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setInteger:selectedViewIndex
					  forKey:@"OEInspectorSelectedViewIndex"];
}

- (void) restoreWindowState:(id) sender
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	if ([userDefaults boolForKey:@"OEInspectorIsVisible"])
		[self showWindow:self];	
}

- (void) storeWindowState:(id) sender
{
	NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	[userDefaults setBool:[[self window] isVisible]
				   forKey:@"OEInspectorIsVisible"];
}

- (void) addExpansion:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"expansion"];
}

- (void) removeExpansion:(id) sender
{
	[self removeDevice:[[fExpansionsController selectedObjects] objectAtIndex:0]];
}

- (void) addStorage:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"storage"];
}

- (void) removeStorage:(id) sender
{
	[self removeDevice:[[fStorageController selectedObjects] objectAtIndex:0]];
}

- (void) addPeripheral:(id) sender
{
	[fDeviceChooserController runModal:self forCategory:@"peripheral"];
}

- (void) removePeripheral:(id) sender
{
	[self removeDevice:[[fPeripheralsController selectedObjects] objectAtIndex:0]];
}

- (void) removeDevice:(NSDictionary *) dict 
{
	[inspectedDocument removeDevice:dict];
}

@end
