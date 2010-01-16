
/**
 * OpenEmulator
 * Mac OS X Device Chooser Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the device chooser window.
 */

#import "DeviceChooserController.h"
#import "Document.h"

@implementation DeviceChooserController

- (id) init
{
	self = [super initWithWindowNibName:@"DeviceChooser"];
	
	if (self)
	{
		deviceChooserViewController = [[DeviceChooserViewController alloc] init];
		[deviceChooserViewController setDelegate:self];
		
		connectorViewController = [[ConnectorViewController alloc] init];
		[connectorViewController setDelegate:self];
		
		currentView = NULL;
		currentStep = 0;
		
		selectedItemOutlets = NULL;
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	[deviceChooserViewController release];
	[connectorViewController release];
}

- (void) updateView:(id) view
{
	if (view == currentView)
		return;
	
	if (currentView)
	{
		[currentView setHidden:YES];
		[currentView removeFromSuperview];
	}
	[fView addSubview:view];
	[view setFrameSize:[fView frame].size];
	[view setHidden:NO];
	
	currentView = view;
}

- (void) setDeviceChooserView
{
	[self updateView:[deviceChooserViewController view]];
	
	[deviceChooserViewController selectItemWithPath:nil];
	
	Document *currentDocument = [fDocumentController currentDocument];
	[deviceChooserViewController updateForInlets:[currentDocument freeInlets]];
	
	[fPreviousButton setEnabled:NO];
	[fNextButton setEnabled:([deviceChooserViewController selectedItemPath]
							 != nil)];
	
	[fMessage setStringValue:
	 NSLocalizedString(@"Choose a template for your new device:",
					   @"Choose a template for your new device:")];
}

- (void) setConnectorView
{
	[self updateView:[connectorViewController view]];
	
	[fPreviousButton setEnabled:YES];
	[fNextButton setEnabled:([deviceChooserViewController selectedItemPath]
							 != nil)];
	
	[fMessage setStringValue:
	 NSLocalizedString(@"Choose an inlet for 'Apple Language Card':",
					   @"Choose an inlet for your new device:")];
	
}

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	[self setWindowFrameAutosaveName:@"DeviceChooser"];
	
	[[self window] center];
	
	[self setDeviceChooserView];
}

- (void) showWindow:(id) sender
{
	[super showWindow:sender];
	
	[[self window] center];
	
	[self setDeviceChooserView];
}

- (void) runModal:(id) sender
{
	[NSApp runModalForWindow:[self window]];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self performNext:sender];
}

- (IBAction) performCancel:(id) sender
{
	[NSApp abortModal];
	
	[[self window] orderOut:self];
}

- (IBAction) performPrevious:(id) sender
{
	currentStep--;
	
	if (currentStep == 0)
		[self setDeviceChooserView];
	else
		[self setConnectorView];
}

- (IBAction) performNext:(id) sender
{
	currentStep++;
	
	if (currentStep == 1)
	{
		if (selectedItemOutlets)
			[selectedItemOutlets release];
		
		selectedItemOutlets = [deviceChooserViewController selectedItemOutlets];
		[selectedItemOutlets retain];
	}
	
	if (currentStep <= [selectedItemOutlets count])
	{
		[self setConnectorView];
	}
	else
	{
		[NSApp stopModal];
		[[self window] orderOut:self];
	}
}

@end
