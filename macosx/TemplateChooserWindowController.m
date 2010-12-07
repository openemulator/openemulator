
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import "TemplateChooserWindowController.h"

@implementation TemplateChooserWindowController

- (id)init
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	
	if (self)
	{
		templateChooserViewController = [[TemplateChooserViewController alloc] init];
		[templateChooserViewController setDelegate:self];
	}
	
	return self;
}

- (void)dealloc
{
	[templateChooserViewController release];
	
	[super dealloc];
}

- (void)run
{
	[self showWindow:self];
	
	[[self window] center];
	
	NSString *group = [templateChooserViewController selectedGroup];
	NSString *path = [templateChooserViewController selectedItemPath];
	if (!path)
	{
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		group = [defaults stringForKey:@"OELastTemplateGroup"];
		path = [defaults stringForKey:@"OELastTemplatePath"];
	}
	[templateChooserViewController selectItemWithPath:path inGroup:group];
	
	[fChooseButton setEnabled:([templateChooserViewController selectedItemPath] != nil)];
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	NSView *view = [templateChooserViewController view];
	[fTemplateChooserView addSubview:view];
	[view setFrameSize:[fTemplateChooserView frame].size];
}

- (void)chooserSelectionDidChange:(id)sender
{
	[fChooseButton setEnabled:([templateChooserViewController selectedItemPath] != nil)];
}

- (void)chooserItemWasDoubleClicked:(id)sender
{
	[self chooseTemplate:sender];
}

- (IBAction)chooseTemplate:(id)sender
{
	NSString *group = [templateChooserViewController selectedGroup];
	NSString *path = [templateChooserViewController selectedItemPath];
	if (path)
	{
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		[defaults setObject:group forKey:@"OELastTemplateGroup"];
		[defaults setObject:path forKey:@"OELastTemplatePath"];
	}
	
	[[self window] orderOut:self];
	
	NSURL *url = [NSURL fileURLWithPath:path];
	if (url)
	{
		NSError *error;
		if (![fDocumentController openUntitledDocumentWithTemplateURL:url
															  display:YES
																error:&error])
			[[NSAlert alertWithError:error] runModal];
	}
}

@end
