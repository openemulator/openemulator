
/**
 * AppleIIGo Mac OS X
 * Controller
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#import "Controller.h"

#define TOOLBAR_INSPECTOR @"Toolbar Inspector"
#define TOOLBAR_PAUSE @"Toolbar Pause"
#define TOOLBAR_RESET @"Toolbar Reset"
#define TOOLBAR_POWEROFF @"Toolbar Poweroff"

@implementation Controller
- (id) init
{
	if (!(self = [super init]))
		return self;
	
	fDefaults = [NSUserDefaults standardUserDefaults];
	
	[NSApp setDelegate:self];
	
	fInfoController = [[InfoWindowController alloc] init];
	
	speechSynthesizer = [[NSSpeechSynthesizer alloc] init];
	
	// Add recents menu
	NSMenu * mainmenu = [NSApp mainMenu];
	NSMenuItem * file = [mainmenu itemWithTitle:@"File"];
	NSMenu * menu = [file submenu];
	NSMenuItem * openRecent = [menu itemWithTitle:@"Open Recent"];
	NSMenu * openRecentMenu = [openRecent submenu];
	
	[openRecentMenu performSelector:@selector(_setMenuName:)
						 withObject:@"NSRecentDocumentsMenu"];
	
	return self;
}

- (void) awakeFromNib
{
    NSToolbar * toolbar = [[NSToolbar alloc]
						   initWithIdentifier:@"AppleIIGoToolbar"];
    [toolbar setDelegate:self];
    [toolbar setAllowsUserCustomization:YES];
    [toolbar setAutosavesConfiguration:YES];
    [toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
    [fWindow setToolbar:toolbar];
    [toolbar release];
    
    [fWindow setDelegate:self]; //do manually to avoid placement issue
    [fWindow makeKeyAndOrderFront:nil];
    
    if ([fDefaults boolForKey:@"InfoWindowIsVisible"])
        [self showInfo:nil];
}

- (void) applicationWillTerminate:(NSNotification *) notification
{
    //remember window states and close all windows
    [fDefaults setBool:[[fInfoController window] isVisible]
				forKey:@"InfoWindowIsVisible"];
	
	[speechSynthesizer release];
}

- (BOOL) validateMenuItem:(NSMenuItem *) menuItem
{
    SEL action = [menuItem action];
    if (action == @selector(showInfo:))
    {
        NSString * title = [[fInfoController window] isVisible] ? 
		NSLocalizedString(@"Hide Inspector", "View menu -> Inspector") :
		NSLocalizedString(@"Show Inspector", "View menu -> Inspector");
        [menuItem setTitle:title];
		
        return YES;
    }
	
	return YES;
}

- (BOOL) application:(NSApplication *)theApplication
			openFile:(NSString *)filename
{
	[[NSDocumentController sharedDocumentController]
	 noteNewRecentDocumentURL:[NSURL fileURLWithPath:filename]];
	
	return YES;
}

- (IBAction) openDocument:(id) sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	NSArray *fileTypes = [NSArray arrayWithObjects:
						  @"emulation",
						  @"wav", @"aiff", @"aif",
						  @"dsk", @"do", @"po", @"nib", @"2mg", @"2img", @"hdv",
						  @"fdi",
						  nil];
	
	[panel setAllowsMultipleSelection:YES];
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	
	[panel beginSheetForDirectory:nil
							 file:nil
							types:fileTypes
				   modalForWindow:fWindow
					modalDelegate:self
				   didEndSelector:@selector(openSheetClosed:returnCode:
											contextInfo:)
					  contextInfo:nil];
}

- (NSArray *) toolbarAllowedItemIdentifiers:(NSToolbar *) toolbar
{
    return [NSMutableArray arrayWithObjects:TOOLBAR_INSPECTOR,
			TOOLBAR_PAUSE, TOOLBAR_RESET, TOOLBAR_POWEROFF,
			NSToolbarSeparatorItemIdentifier,
			NSToolbarSpaceItemIdentifier,
			NSToolbarFlexibleSpaceItemIdentifier,
			NSToolbarCustomizeToolbarItemIdentifier, nil];
}

- (NSArray *) toolbarDefaultItemIdentifiers:(NSToolbar *) toolbar
{
    return [NSMutableArray arrayWithObjects:TOOLBAR_POWEROFF,
			NSToolbarSpaceItemIdentifier,
			TOOLBAR_RESET, TOOLBAR_PAUSE,
			NSToolbarFlexibleSpaceItemIdentifier,
			TOOLBAR_INSPECTOR,
			nil];
}

- (NSToolbarItem *) toolbar:(NSToolbar *) toolbar
	  itemForItemIdentifier:(NSString *) ident
  willBeInsertedIntoToolbar:(BOOL) flag
{
	NSToolbarItem * item = nil;
    if ([ident isEqualToString:TOOLBAR_PAUSE])
    {
		item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
        [item setLabel:@"Pause"];
        [item setPaletteLabel:[item label]];
        [item setToolTip:@"Pause"];
        [item setImage:[NSImage imageNamed:@"TBPause.png"]];
        [item setTarget:self];
        [item setAction:@selector(showInfo:)];
    }
    else if ([ident isEqualToString:TOOLBAR_RESET])
    {
		item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
        [item setLabel:@"Reset"];
        [item setPaletteLabel:[item label]];
        [item setToolTip:@"Reset"];
        [item setImage:[NSImage imageNamed:@"TBReset.png"]];
        [item setTarget:self];
        [item setAction:@selector(showInfo:)];
    }
    else if ([ident isEqualToString:TOOLBAR_POWEROFF])
    {
		item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
        [item setLabel:@"Power"];
        [item setPaletteLabel:[item label]];
        [item setToolTip:@"Power off or power on the emulation."];
        [item setImage:[NSImage imageNamed:@"TBPower.png"]];
        [item setTarget:self];
        [item setAction:@selector(showInfo:)];
    }
    else if ([ident isEqualToString:TOOLBAR_INSPECTOR])
    {
		item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
        [item setLabel:@"Inspector"];
        [item setPaletteLabel:[item label]];
        [item setToolTip:@"Show or hide the Inspector window."];
        [item setImage:[NSImage imageNamed:@"TBInspector.png"]];
        [item setTarget:self];
        [item setAction:@selector(showInfo:)];
    }
	
	return [item autorelease];
}

- (IBAction) showInfo:(id) sender
{
    if ([[fInfoController window] isVisible])
        [fInfoController close];
    else
    {
        [fInfoController updateInfoStats];
        [[fInfoController window] orderFront:nil];
    }
}

- (IBAction) linkHomepage:(id) sender
{
	NSURL *url = [NSURL	URLWithString:@APPLEIIGO_HOMEPAGE];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) linkForums:(id) sender
{
	NSURL *url = [NSURL	URLWithString:@APPLEIIGO_FORUMSURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) linkDevelopment:(id) sender
{
	NSURL *url = [NSURL	URLWithString:@APPLEIIGO_DEVURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) linkDonate:(id) sender
{
	NSURL *url = [NSURL	URLWithString:@APPLEIIGO_DONATEURL];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

@end
