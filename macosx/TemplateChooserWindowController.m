
/**
 * OpenEmulator
 * Mac OS X Template Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the template chooser window.
 */

#import "TemplateChooserWindowController.h"
#import "TemplateChooserViewController.h"
#import "DocumentController.h"

@implementation TemplateChooserWindowController

- (id)init
{
	self = [super initWithWindowNibName:@"TemplateChooser"];
	
	return self;
}

- (void)dealloc
{
	[templateChooserViewController release];
	
	[super dealloc];
}

- (void)run
{
	[templateChooserViewController reloadData];
	
	[self showWindow:self];
	
	[[self window] center];
}

- (void)windowDidLoad
{
	[self setWindowFrameAutosaveName:@"TemplateChooser"];
	
	templateChooserViewController = [[TemplateChooserViewController alloc] init];
	[templateChooserViewController setDelegate:self];
	
	NSView *view = [templateChooserViewController view];
	[view setFrameSize:[fTemplateChooserView frame].size];
	[fTemplateChooserView addSubview:view];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *group = [defaults stringForKey:@"OELastTemplateGroup"];
	NSString *path = [defaults stringForKey:@"OELastTemplatePath"];
	[templateChooserViewController selectGroup:group andItemWithPath:path];
}

- (void)templateChooserSelectionDidChange:(id)sender
{
	NSString *path = [templateChooserViewController selectedItemPath];
	
	[fChooseButton setEnabled:(path != nil)];
}

- (void)templateChooserItemWasDoubleClicked:(id)sender
{
	[self chooseTemplate:sender];
}

- (IBAction)chooseTemplate:(id)sender
{
	[[self window] orderOut:self];
	
	NSString *group = [templateChooserViewController selectedGroup];
	NSString *path = [templateChooserViewController selectedItemPath];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:group forKey:@"OELastTemplateGroup"];
	[defaults setObject:path forKey:@"OELastTemplatePath"];
	
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
