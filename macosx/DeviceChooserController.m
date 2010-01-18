
/**
 * OpenEmulator
 * Mac OS X Device Chooser Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
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
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
	
	if (selectedItemOutlets)
		[selectedItemOutlets release];
	
	[deviceChooserViewController release];
	[connectorViewController release];
}

- (void) updateView:(id) view
			  title:(NSString *) title
	previousEnabled:(BOOL) previousEnabled
		   lastStep:(BOOL) lastStep
{
	if (currentView)
	{
		[currentView setHidden:YES];
		[currentView removeFromSuperview];
	}
	[fView addSubview:view];
	[view setFrameSize:[fView frame].size];
	[view setHidden:NO];
	
	currentView = view;
	
	[fMessage setStringValue:title];
	
	[fPreviousButton setEnabled:previousEnabled];
	if (lastStep)
		[fNextButton setTitle:NSLocalizedString(@"Finish", @"Finish")];
	else
		[fNextButton setTitle:NSLocalizedString(@"Next", @"Next")];
}

- (void) setDeviceChooserView
{
	[self updateView:[deviceChooserViewController view]
			   title:NSLocalizedString(@"Choose a template for your new device:",
									   @"Choose a template for your new device:")
	 previousEnabled:NO
			lastStep:NO
	 ];
}

- (void) setConnectorViewAtIndex:(int) index
{
	NSString *label;
	label = [[selectedItemOutlets objectAtIndex:index] objectForKey:@"label"];
	
	[self updateView:[connectorViewController view]
			   title:[NSString localizedStringWithFormat:
					  @"Choose an inlet for \u201C%@\u201D:", label,
					  @"Choose an inlet for \u201C%@\u201D:"]
	 previousEnabled:YES
			lastStep:(index == ([selectedItemOutlets count] - 1))
	 ];
	
	[connectorViewController updateWithIndex:index];
}

- (void) setup
{
	currentStep = 0;
	[self setDeviceChooserView];
	
	NSArray *freeInlets = [[fDocumentController currentDocument] freeInlets];
	[deviceChooserViewController updateWithInlets:freeInlets];
	
	[fNextButton setEnabled:([deviceChooserViewController selectedItemPath] != nil)];
}

- (void) windowDidLoad
{
	[super windowDidLoad];
	
	[self setWindowFrameAutosaveName:@"DeviceChooser"];
	
	[self setup];
}

- (void) runModal:(id) sender
{
	if ([self isWindowLoaded])
		[self setup];
	
	[NSApp runModalForWindow:[self window]];
}

- (void) chooserWasDoubleClicked:(id) sender
{
	[self performNext:sender];
}

- (void) connectorWasDoubleClicked:(id) sender
{
	[self performNext:sender];
}

- (IBAction) performCancel:(id) sender
{
	[[self window] orderOut:self];
	
	[NSApp abortModal];
}

- (IBAction) performPrevious:(id) sender
{
	currentStep--;
	
	if (currentStep == 0)
		[self setDeviceChooserView];
	else
		[self setConnectorViewAtIndex:(currentStep - 1)];
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
		
		NSArray *freeInlets = [[fDocumentController currentDocument] freeInlets];
		
		[connectorViewController setupWithOutlets:selectedItemOutlets
										andInlets:freeInlets];
	}
	
	if (currentStep <= [selectedItemOutlets count])
		[self setConnectorViewAtIndex:(currentStep - 1)];
	else
	{
		[self performCancel:sender];
		
		// Call addDML
	}
}

@end
