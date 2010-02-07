
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
	
	[deviceChooserViewController release];
	[connectorViewController release];
	
	if (selectedItemOutlets)
		[selectedItemOutlets release];
	if (selectedItemInlets)
		[selectedItemInlets release];
}

- (void) updateView:(id) view
			  title:(NSString *) title
	previousEnabled:(BOOL) previousEnabled
		   lastStep:(BOOL) lastStep
{
	if (view != currentView)
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
	}
	
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
	
	NSMutableDictionary *outlet = [selectedItemOutlets objectAtIndex:index];
	NSString *outletType = [outlet objectForKey:@"type"];
	
	NSArray *freeInlets = [[fDocumentController currentDocument] freeInlets];
	NSMutableArray *inlets = [NSMutableArray array];
	
	for (int i = 0; i < [freeInlets count]; i++)
	{
		NSMutableDictionary *inlet = [freeInlets objectAtIndex:i];
		NSString *inletType = [inlet objectForKey:@"type"];
		
		if ([inletType compare:outletType] != NSOrderedSame)
			continue;
		
		NSString *inletRef = [inlet objectForKey:@"ref"];
		BOOL isAvailable = YES;
		for (int j = 0; j < [selectedItemInlets count]; j++)
		{
			NSString *prevInletRef = [selectedItemInlets objectAtIndex:j];
			
			if ([prevInletRef compare:inletRef] == NSOrderedSame)
			{
				isAvailable = NO;
				break;
			}
		}

		if (!isAvailable)
			continue;
		
		[inlets addObject:inlet];
	}
	
	[connectorViewController updateWithInlets:inlets];
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

- (void) runModal:(id) sender forInletsOfType:(NSString *) type
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
	{
		NSRange theRange;
		theRange.location = currentStep - 1;
		theRange.length = [selectedItemInlets count] - theRange.location;
		[selectedItemInlets removeObjectsInRange:theRange];
		
		[self setConnectorViewAtIndex:(currentStep - 1)];
	}
}

- (IBAction) performNext:(id) sender
{
	if (currentStep == 0)
	{
		if (selectedItemOutlets)
			[selectedItemOutlets release];
		selectedItemOutlets = [deviceChooserViewController selectedItemOutlets];
		[selectedItemOutlets retain];
		
		if (selectedItemInlets)
			[selectedItemInlets release];
		selectedItemInlets = [[NSMutableArray alloc] init];
	}
	else
	{
		NSRange range;
		range.location = currentStep - 1;
		range.length = [selectedItemInlets count] - range.location;
		[selectedItemInlets removeObjectsInRange:range];
		
		NSString *inletRef = [connectorViewController selectedInletRef];
		[selectedItemInlets addObject:inletRef];
	}
	
	currentStep++;
	
	if (currentStep <= [selectedItemOutlets count])
		[self setConnectorViewAtIndex:(currentStep - 1)];
	else
	{
		[self performCancel:sender];
		
		NSString *dmlPath = [deviceChooserViewController selectedItemPath];
		NSMutableDictionary *connections = [NSMutableDictionary dictionary];
		for (int i = 0; i < [selectedItemOutlets count]; i++)
		{
			NSString *inletRef = [selectedItemInlets objectAtIndex:i];
			NSString *outletRef = [[selectedItemOutlets objectAtIndex:i]
								   objectForKey:@"ref"];
			[connections setObject:outletRef forKey:inletRef];
		}
		
		[[fDocumentController currentDocument] addDevices:dmlPath
											  connections:connections];
	}
}

@end
