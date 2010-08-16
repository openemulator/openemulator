
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import "Document.h"
#import "DocumentController.h"
#import "DocumentWindowController.h"

#import "OEInfo.h"
#import "OEPAEmulation.h"
#import "Host.h"

#import "StringConversion.h"

@implementation Document

- (id)init
{
	if (self = [super init])
	{
		emulation = nil;
		
		image = nil;
		label = nil;
		notes = nil;
		modificationDate = nil;
		powerState = nil;
		
		freeInlets = [[NSMutableArray alloc] init];
		
		expansions = [[NSMutableArray alloc] init];
		storage = [[NSMutableArray alloc] init];
		peripherals = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	if ([self init])
	{
		if ([self readFromURL:absoluteURL
					   ofType:nil
						error:outError])
			return self;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return nil;
}

- (void)dealloc
{
	[self deleteEmulation];
	
	[freeInlets release];
	
	[expansions release];
	[storage release];
	[peripherals release];
	
	[super dealloc];
}

- (void)constructEmulation:(NSURL *)url
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	OEPA *oepa = (OEPA *)[documentController oepa];
	
	string path = getString([url path]);
	string resourcePath = getString([[NSBundle mainBundle] resourcePath]);
	
	emulation = new OEPAEmulation(oepa, path, resourcePath);
	
	[documentController addEmulation:emulation];
}

- (void)deleteEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	[documentController removeEmulation:emulation];
	
	delete (OEPAEmulation *)emulation;
	
	emulation = nil;
}

- (void *)emulation
{
	return emulation;
}

- (BOOL)setHostProperty:(NSString *)name
				  value:(NSString *)value
{
	if (!emulation)
		return false;
	
	return ((OEPAEmulation *)emulation)->setProperty(HOST_DEVICE,
													 getString(name),
													 getString(value));
}

- (NSString *)getHostProperty:(NSString *)name
{
	if (!emulation)
		return nil;
	
	string value;
	if (((OEPAEmulation *)emulation)->getProperty(HOST_DEVICE,
												  getString(name),
												  value))
		return getNSString(value);
	
	return @"";
}

- (void)notifyHost:(int)notification data:(void *)data
{
	((OEPAEmulation *)emulation)->notify(HOST_DEVICE, notification, data);
}

- (int)postHostEvent:(int)message data:(void *)data
{
	return ((OEPAEmulation *)emulation)->postEvent(HOST_DEVICE, message, data);
}

- (NSImage *)getResourceImage:(NSString *)imagePath
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	NSString *path = [[resourcePath
					   stringByAppendingString:@"/images/"]
					  stringByAppendingString:imagePath];
	NSImage *theImage = [[NSImage alloc] initWithContentsOfFile:path];
	if (theImage)
		[theImage autorelease];
	
	return theImage;
}

- (NSAttributedString *)formatDeviceLabel:(NSString *)deviceLabel
					  withInformativeText:(NSString *)informativeText
{
	NSMutableParagraphStyle *paragraphStyle;
	paragraphStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy]
					  autorelease];
	[paragraphStyle setLineBreakMode:NSLineBreakByTruncatingTail];
	
	NSDictionary *deviceLabelAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
									  [NSFont messageFontOfSize:12.0f],
									  NSFontAttributeName,
									  paragraphStyle,
									  NSParagraphStyleAttributeName,
									  [NSColor controlTextColor],
									  NSForegroundColorAttributeName,
									  nil];
	NSMutableAttributedString *aString;
	aString = [[[NSMutableAttributedString alloc] initWithString:deviceLabel
													  attributes:deviceLabelAttrs]
			   autorelease];
	
	NSDictionary *informativeTextAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
										  [NSFont messageFontOfSize:9.0f],
										  NSFontAttributeName,
										  paragraphStyle,
										  NSParagraphStyleAttributeName,
										  [NSColor darkGrayColor],
										  NSForegroundColorAttributeName,
										  nil];
	NSAttributedString *aInformativeText;
	aInformativeText = [[[NSAttributedString alloc] initWithString:informativeText
														attributes:informativeTextAttrs]
						autorelease];
	[aString appendAttributedString:aInformativeText];
	
	return aString;
}

- (void)updatePowerState
{
	NSString *property = [self getHostProperty:@"powerState"];
	int value = [property intValue];
	
	switch (value)
	{
		case HOST_POWERSTATE_ON:
			[self setPowerState:
			 NSLocalizedString(@"Powered on", @"Powered on")];
			break;
		case HOST_POWERSTATE_PAUSE:
			[self setPowerState:
			 NSLocalizedString(@"Paused", @"Paused")];
			break;
		case HOST_POWERSTATE_STANDBY:
			[self setPowerState:
			 NSLocalizedString(@"Standby", @"Standby")];
			break;
		case HOST_POWERSTATE_SLEEP:
			[self setPowerState:
			 NSLocalizedString(@"Suspended", @"Suspended")];
			break;
		case HOST_POWERSTATE_HIBERNATE:
			[self setPowerState:
			 NSLocalizedString(@"Hibernated", @"Hibernated")];
			break;
		case HOST_POWERSTATE_OFF:
			[self setPowerState:
			 NSLocalizedString(@"Powered off", @"Powered off")];
			break;
		default:
			[self setPowerState:
			 NSLocalizedString(@"Unknown", @"Unknown power state")];
			break;
	}
}

- (void)updateDevices
{
	int count;
	
	// Clean up
	[freeInlets removeAllObjects];
	
	count = [expansions count];
	for (int i = 0; i < count; i++)
		[self removeObjectFromExpansionsAtIndex:0];
	count = [storage count];
	for (int i = 0; i < count; i++)
		[self removeObjectFromStorageAtIndex:0];
	count = [peripherals count];
	for (int i = 0; i < count; i++)
		[self removeObjectFromPeripheralsAtIndex:0];
	
	// Process inlets
	OEPorts *inlets = ((OEPAEmulation *)emulation)->getInlets();
	for (OEPorts::iterator inlet = inlets->begin();
		 inlet != inlets->end();
		 inlet++)
	{
		if ((*inlet)->connection)
			continue;
		
		NSString *ref = getNSString((*inlet)->ref);
		NSString *type = getNSString((*inlet)->type);
		NSString *labelName = getNSString((*inlet)->label);
		NSString *imageName = getNSString((*inlet)->image);
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:ref forKey:@"ref"];
		[dict setObject:type forKey:@"type"];
		[dict setObject:labelName forKey:@"label"];
		[dict setObject:imageName forKey:@"image"];
		
		[freeInlets addObject:dict];
	}
	
	// Process devices
	int expansionIndex = 0;
	int storageIndex = 0;
	int peripheralIndex = 0;
	OEDevices *devices = ((OEPAEmulation *)emulation)->getDevices();
	for (OEDevices::iterator device = devices->begin();
		 device != devices->end();
		 device++)
	{
		NSString *deviceName = getNSString((*device)->name);
		NSString *deviceLabel = getNSString((*device)->label);
		NSString *deviceImage = getNSString((*device)->image);
		NSString *connectionLabel = getNSString((*device)->connectionLabel);
		
		NSImage *theImage = [self getResourceImage:deviceImage];
		
		NSString *informativeText = @"";
		if ([connectionLabel length] > 0)
			informativeText = [NSString localizedStringWithFormat:@"\n(on %@)",
							   connectionLabel];
		NSAttributedString *theTitle = [self formatDeviceLabel:deviceLabel
										   withInformativeText:informativeText];
		
		BOOL isRemovable = ((*device)->label == "static");
		NSNumber *removable = [NSNumber numberWithBool:isRemovable];
		
		NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
							  theTitle, @"title",
							  theImage, @"image",
							  deviceLabel, @"label",
							  deviceName, @"name",
							  removable, @"removable",
							  nil];
		
		if ((*device)->type == "expansion")
			[self insertObject:dict inExpansionsAtIndex:expansionIndex++];
		else if ((*device)->type == "storage")
			[self insertObject:dict inStorageAtIndex:storageIndex++];
		else if ((*device)->type == "peripheral")
			[self insertObject:dict inPeripheralsAtIndex:peripheralIndex++];
	}
	
	return;
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self deleteEmulation];
	[self constructEmulation:absoluteURL];
	
	if (emulation)
	{
		if (((OEPAEmulation *)emulation)->isOpen())
		{
			string value;
			
			value = ((OEPAEmulation *)emulation)->getLabel();
			[self setLabel:getNSString(value)];
//			value = ((OEPAEmulation *)emulation)->getNotes();
//			[self setNotes:getNSString(value)];
			[self updatePowerState];
			value = ((OEPAEmulation *)emulation)->getImage();
			[self setImage:[self getResourceImage:getNSString(value)]];
			
			[self updateDevices];
			
			return YES;
		}
		
		[self deleteEmulation];
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return NO;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL
			ofType:(NSString *)typeName
			 error:(NSError **)outError
{
	if (emulation)
	{
		string emulationPath = getString([[absoluteURL path]
										  stringByAppendingString:@"/"]);
		
		[self setHostProperty:@"notes" value:[self notes]];
		
		if (((OEPAEmulation *)emulation)->save(emulationPath))
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (void)setFileModificationDate:(NSDate *)date
{
	[super setFileModificationDate:date];
	
	NSString *value;
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	[dateFormatter setDateStyle:NSDateFormatterMediumStyle];
	[dateFormatter setTimeStyle:NSDateFormatterMediumStyle];
	value = [dateFormatter stringFromDate:date];
	[dateFormatter release];
	
	[self setModificationDate:value];
}

- (IBAction)saveDocumentAsTemplate:(id)sender
{
	NSString *path = [TEMPLATE_FOLDER stringByExpandingTildeInPath];
	
	NSFileManager *fileManager = [NSFileManager defaultManager];
	if (![fileManager fileExistsAtPath:path])
		[fileManager createDirectoryAtPath:path
			   withIntermediateDirectories:YES
								attributes:nil
									 error:nil];
	
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setRequiredFileType:@"emulation"];
	[panel beginSheetForDirectory:path
							 file:nil
				   modalForWindow:[self windowForSheet]
					modalDelegate:self
				   didEndSelector:@selector(saveDocumentAsTemplateDidEnd:
											returnCode:contextInfo:)
					  contextInfo:nil];
}

- (void)saveDocumentAsTemplateDidEnd:(NSSavePanel *)panel
						  returnCode:(int)returnCode
						 contextInfo:(void *)contextInfo
{
	if (returnCode != NSOKButton)
		return;
	
	NSError *error;
	if (![self writeToURL:[panel URL]
				   ofType:nil
					error:&error])
		[[NSAlert alertWithError:error] runModal];
}

- (void)addDevices:(NSString *)path
	   connections:(NSDictionary *)connections
{
	string pathString = getString(path);
	map<string, string> connectionsMap;
	
	NSEnumerator *i = [connections keyEnumerator];
	NSString *inletRef;
	
	while (inletRef = [i nextObject])
	{
		NSString *outletRef = [connections objectForKey:inletRef];
		
		string inletRefString = getString(inletRef);
		string outletRefString = getString(outletRef);
		
		connectionsMap[inletRefString] = outletRefString;
	}
	
	if (!((OEPAEmulation *)emulation)->addDML(pathString, connectionsMap))
	{
		NSString *messageText = @"The device could not be added.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
	}
	
	[self updateDevices];
	[self updateChangeCount:NSChangeDone];
}

- (void)removeDevice:(NSDictionary *)dict
{
	NSString *deviceRef = [dict objectForKey:@"ref"];
	NSString *deviceLabel = [dict objectForKey:@"label"];
	
	string refString = getString(deviceRef);
	
/*	if (!((OEPAEmulation *)emulation)->isDeviceTerminal(refString))
	{
		NSString *messageText = @"Do you want to remove the device \u201C%@\u201D?";
		NSString *informativeText = @"There is one or more devices connected to it, "
		"which will be removed as well.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:[NSString localizedStringWithFormat:messageText,
							   deviceLabel, messageText]];
		[alert setInformativeText:NSLocalizedString(informativeText,
													informativeText)];
		[alert addButtonWithTitle:NSLocalizedString(@"OK", @"OK")];
		[alert addButtonWithTitle:NSLocalizedString(@"Cancel", @"Cancel")];
		if ([alert runModal] != NSAlertDefaultReturn)
			return;
	}*/
	
	if (!((OEPAEmulation *)emulation)->removeDevice(refString))
	{
		NSString *messageText = @"The device could not be removed.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert runModal];
	}
	
	[self updateDevices];
	[self updateChangeCount:NSChangeDone];
}

- (void)makeWindowControllers
{
	NSWindowController *windowController;
	
	windowController = [[DocumentWindowController alloc] init];
	[self addWindowController:windowController];
	[windowController release];
}

- (NSImage *)image
{
	return [[image retain] autorelease];
}

- (void)setImage:(NSImage *)value
{
    if (image != value)
	{
        [image release];
        image = [value copy];
    }
}

- (NSString *)label
{
	return [[label retain] autorelease];
}

- (void)setLabel:(NSString *)value
{
    if (label != value)
	{
        [label release];
        label = [value copy];
    }
}

- (NSString *)notes
{
	return [[notes retain] autorelease];
}

- (void)setNotes:(NSString *)value
{
    if (notes != value)
	{
        [notes release];
        notes = [value copy];
    }
}

- (NSString *)modificationDate
{
	return modificationDate;
}

- (void)setModificationDate:(NSString *)value
{
    if (modificationDate != value)
	{
        [modificationDate release];
        modificationDate = [value copy];
    }
}

- (NSString *)powerState
{
	return powerState;
}

- (void)setPowerState:(NSString *)value
{
    if (powerState != value)
	{
        [powerState release];
        powerState = [value copy];
    }
}

- (NSMutableArray *)freeInlets
{
	return freeInlets;
}

- (NSMutableArray *)expansions
{
	return [[expansions retain] autorelease];
}

- (void)insertObject:(id)value inExpansionsAtIndex:(NSUInteger)index
{
    [expansions insertObject:value atIndex:index];
}

- (void)removeObjectFromExpansionsAtIndex:(NSUInteger)index
{
    [expansions removeObjectAtIndex:index];
}

- (NSMutableArray *)storage
{
	return [[storage retain] autorelease];
}

- (void)insertObject:(id)value inStorageAtIndex:(NSUInteger)index
{
    [storage insertObject:value atIndex:index];
}

- (void)removeObjectFromStorageAtIndex:(NSUInteger)index
{
    [storage removeObjectAtIndex:index];
}

- (NSMutableArray *)peripherals
{
	return [[peripherals retain] autorelease];
}

- (void)insertObject:(id)value inPeripheralsAtIndex:(NSUInteger)index
{
    [peripherals insertObject:value atIndex:index];
}

- (void)removeObjectFromPeripheralsAtIndex:(NSUInteger)index
{
    [peripherals removeObjectAtIndex:index];
}

- (NSString *)documentText
{
	string characterString;
	[self notifyHost:HOST_CLIPBOARD_WILL_COPY data:&characterString];
	
	return getNSString(characterString);
}

- (void)copy:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
	
	[pasteboard declareTypes:pasteboardTypes owner:self];
	[pasteboard setString:[self documentText] forType:NSStringPboardType];
}

- (void)paste:(id)sender
{
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	
	string characterString = getString([pasteboard stringForType:NSStringPboardType]);
	
	[self notifyHost:HOST_CLIPBOARD_WILL_PASTE data:&characterString];
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
	[dummy insertText:[self documentText]];
	[dummy startSpeaking:self];
}

@end
