
/**
 * OpenEmulator
 * Mac OS X Preferences Window Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the preferences window
 */

#import "PreferencesWindowController.h"
#import "TemplateChooserViewController.h"

@implementation PreferencesWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"Preferences"];
    
    return self;
}

- (void)dealloc
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults removeObserver:self
                      forKeyPath:@"OEDefaultTemplatePath"];
    
    [templateChooserViewController release];
    
    [super dealloc];
}

- (void)windowDidLoad
{
    NSToolbar *toolbar = [[NSToolbar alloc] initWithIdentifier:@"Preferences Toolbar"];
    NSString *selectedItemIdentifier = @"General";
    
    [toolbar setDelegate:self];
    [toolbar setAllowsUserCustomization:NO];
    [toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
    [toolbar setSizeMode:NSToolbarSizeModeRegular];
    [toolbar setSelectedItemIdentifier:selectedItemIdentifier];
    [[self window] setToolbar:toolbar];
    [toolbar release];
    
    [self updateGeneralView];
    
    [self setView:selectedItemIdentifier];
    
    templateChooserViewController = [[TemplateChooserViewController alloc] init];
    [templateChooserViewController setDelegate:self];
    
    NSView *view = [templateChooserViewController view];
    [view setFrameSize:[fTemplateChooserView frame].size];
    [fTemplateChooserView addSubview:view];
    
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults addObserver:self
                   forKeyPath:@"OEDefaultTemplatePath"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    if ([keyPath isEqualToString:@"OEDefaultTemplatePath"])
        [self updateGeneralView];
}



- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
     itemForItemIdentifier:(NSString *)ident
 willBeInsertedIntoToolbar:(BOOL)flag
{
    NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:ident];
    
    if (!item)
        return nil;
    
    if ([ident isEqualToString:@"General"])
    {
        [item setLabel:NSLocalizedString(@"General", @"Preferences Toolbar.")];
        [item setImage:[NSImage imageNamed:@"IconGeneral.png"]];
        [item setTarget:self];
        [item setAction:@selector(selectView:)];
    }
    else if ([ident isEqualToString:@"Audio"])
    {
        [item setLabel:NSLocalizedString(@"Audio", @"Preferences Toolbar.")];
        [item setImage:[NSImage imageNamed:@"IconAudio.png"]];
        [item setTarget:self];
        [item setAction:@selector(selectView:)];
    }
    else if ([ident isEqualToString:@"Video"])
    {
        [item setLabel:NSLocalizedString(@"Video", @"Preferences Toolbar.")];
        [item setImage:[NSImage imageNamed:@"IconVideo.png"]];
        [item setTarget:self];
        [item setAction:@selector(selectView:)];
    }
    
    [item autorelease];
    
    return item;
}

- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar
{
    return [NSArray arrayWithObjects:@"General", @"Audio", @"Video", nil];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
    return [self toolbarSelectableItemIdentifiers:toolbar];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
    return [self toolbarSelectableItemIdentifiers:toolbar];
}



- (void)selectView:(id)sender
{
    [self setView:[sender itemIdentifier]];
}

- (void)setView:(NSString *)itemIdentifier
{
    NSView *view;
    if ([itemIdentifier isEqualToString:@"General"])
        view = fGeneralView;
    else if ([itemIdentifier isEqualToString:@"Audio"])
        view = fAudioView;
    else if ([itemIdentifier isEqualToString:@"Video"])
        view = fVideoView;
    else
        view = fGeneralView;
    
    NSWindow *window = [self window];
    if ([window contentView] == view)
        return;
    
    [view setHidden:YES];
    
    NSRect windowRect = [window frame];
    float difference = (([view frame].size.height - 
                         [[window contentView] frame].size.height) *
                        [window userSpaceScaleFactor]);
    windowRect.origin.y -= difference;
    windowRect.size.height += difference;
    
    [window setContentView:view];
    [window setFrame:windowRect display:YES animate:YES];
    [view setHidden:NO];
    
    [window setTitle:NSLocalizedString(itemIdentifier, @"Preferences.")];
}

- (void)updateGeneralView
{
    [self setUseTemplate:[[NSUserDefaults standardUserDefaults]
                                     boolForKey:@"OEDefaultTemplateEnabled"]];
}

- (void)setUseTemplate:(BOOL)useTemplate
{
    NSString *useTemplateTitle = NSLocalizedString(@"Use template: ",
                                                   @"Preferences.");
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *defaultTemplatePath = [defaults stringForKey:@"OEDefaultTemplatePath"];
    
    if (defaultTemplatePath)
    {
        NSString *defaultTemplateName = [[defaultTemplatePath lastPathComponent]
                                         stringByDeletingPathExtension];
        useTemplateTitle = [useTemplateTitle stringByAppendingString:defaultTemplateName];
    }
    
    [fShowTemplateChooserRadio setIntValue:!useTemplate];
    [fUseTemplateRadio setIntValue:useTemplate];
    [fUseTemplateRadio setTitle:useTemplateTitle];
    [fChooseTemplateButton setEnabled:useTemplate];
    
    if ((defaultTemplatePath == nil) && useTemplate)
        [self openTemplateChooser:self];
    else
        [defaults setBool:useTemplate
                   forKey:@"OEDefaultTemplateEnabled"];
}

- (IBAction)useTemplateDidChange:(id)sender
{
    [self setUseTemplate:[[sender selectedCell] tag]];
}

- (IBAction)openTemplateChooser:(id)sender
{
    [templateChooserViewController reloadData];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *group = [defaults stringForKey:@"OEDefaultTemplateGroup"];
    NSString *path = [defaults stringForKey:@"OEDefaultTemplatePath"];
    [templateChooserViewController selectGroup:group andItemWithPath:path];
    
    [NSApp beginSheet:fTemplateChooserSheet
       modalForWindow:[self window]
        modalDelegate:self
       didEndSelector:@selector(didEndSheet:returnCode:contextInfo:)
          contextInfo:nil];
}

- (void)templateChooserSelectionDidChange:(id)sender
{
    [fTemplateChooserChooseButton setEnabled:
     ([templateChooserViewController selectedItemPath] != nil)];
}

- (void)templateChooserItemWasDoubleClicked:(id)sender
{
    [self chooseTemplate:sender];
}

- (IBAction)chooseTemplate:(id)sender
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:[templateChooserViewController selectedGroup]
                 forKey:@"OEDefaultTemplateGroup"];
    [defaults setObject:[templateChooserViewController selectedItemPath]
                 forKey:@"OEDefaultTemplatePath"];
    [defaults setBool:YES
               forKey:@"OEDefaultTemplateEnabled"];
    
    [self closeTemplateChooser:sender];
}

- (IBAction)closeTemplateChooser:(id)sender
{
    [NSApp endSheet:fTemplateChooserSheet];
}

- (void)didEndSheet:(NSWindow *)sheet
         returnCode:(int)returnCode
        contextInfo:(void *)contextInfo
{ 
    [sheet orderOut:self];
    
    [self updateGeneralView];
}

@end
