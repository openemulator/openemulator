
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import "Document.h"
#import "DocumentWindowController.h"

#import "OEEmulation.h"
#import "OEInfo.h"

#import "oepa.h"

@implementation Document

- (id) init
{
	if (self = [super init])
	{
		emulation = nil;
		
		pasteboard = [NSPasteboard generalPasteboard];
		pasteboardTypes = [[NSArray alloc] initWithObjects:
						   NSStringPboardType,
						   nil];
		
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

- (id) initWithTemplateURL:(NSURL *) absoluteURL
					 error:(NSError **) outError
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

- (void) dealloc
{
	[super dealloc];
	
	if (emulation)
		oepaDestroy(emulation);
	
	[pasteboardTypes release];
	
	[freeInlets release];
	
	[expansions release];
	[storage release];
	[peripherals release];
}

- (NSString *) getDMLProperty:(NSString *) name
{
	if (!emulation)
		return nil;
	
	xmlDocPtr dml = oepaGetDML(emulation);
	
	xmlNodePtr rootNode = xmlDocGetRootElement(dml);
	
	xmlChar *valuec = xmlGetProp(rootNode, BAD_CAST [name UTF8String]);
	NSString *value = [NSString stringWithUTF8String:(const char *) valuec];
	xmlFree(valuec);
	
	return value;
}

- (BOOL) setComponentProperty:(NSString *) name
							ref:(NSString *) ref
							value:(NSString *) value
{
	if (!emulation)
		return false;
	
	return oepaSetProperty(emulation,
						   string([ref UTF8String]),
						   string([name UTF8String]),
						   string([value UTF8String]));
}

- (NSString *) getComponentProperty:(NSString *) name
								ref:(NSString *) ref
{
	if (!emulation)
		return nil;
	
	string value;
	if (oepaGetProperty(emulation,
						string([ref UTF8String]),
						string([name UTF8String]),
						value))
		return [NSString stringWithUTF8String:value.c_str()];
			
	return @"";
}

- (NSImage *) getResourceImage:(NSString *) imagePath
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

- (NSAttributedString *) formatDeviceLabel:(NSString *) deviceLabel
					   withInformativeText:(NSString *) informativeText
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
						 attributes:informativeTextAttrs] autorelease];
	[aString appendAttributedString:aInformativeText];
	
	return aString;
}

- (void) updatePowerState
{
	NSString *property = [self getComponentProperty:@"powerState" ref:@"host::host"];
	int value = [property intValue];
	
	switch (value)
	{
		case HOST_POWERSTATE_ON:
			[self setPowerState:
			 NSLocalizedString(@"Powered on", @"Powered on")];
			break;
		case HOST_POWERSTATE_PAUSED:
			[self setPowerState:
			 NSLocalizedString(@"Paused", @"Paused")];
			break;
		case HOST_POWERSTATE_STANDBY:
			[self setPowerState:
			 NSLocalizedString(@"Standby", @"Standby")];
			break;
		case HOST_POWERSTATE_SLEEP:
			[self setPowerState:
			 NSLocalizedString(@"Suspended", @"Sleep")];
			break;
		case HOST_POWERSTATE_HIBERNATE:
			[self setPowerState:
			 NSLocalizedString(@"Hibernate", @"Hibernate")];
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

- (void) updateDevices
{
	int count;
	
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
	
	// Process info
	xmlDocPtr dmlDocPtr = oepaGetDML(emulation);
	OEInfo info(dmlDocPtr);
	if (!info.isLoaded())
		return;
	
	// Process inlets
	OEPorts *inlets = info.getInlets();
	for (OEPorts::iterator i = inlets->begin();
		 i != inlets->end();
		 i++)
	{
		if (i->connectionPort)
			continue;
		
		string stringRef = i->ref.getStringRef();
		
		NSString *portRef = [NSString stringWithUTF8String:stringRef.c_str()];
		NSString *portType = [NSString stringWithUTF8String:i->type.c_str()];
		NSString *portCategory = [NSString stringWithUTF8String:i->category.c_str()];
		NSString *portLabel = [NSString stringWithUTF8String:i->connectionLabel.c_str()];
		NSString *portImage = [NSString stringWithUTF8String:i->image.c_str()];
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:portRef forKey:@"ref"];
		[dict setObject:portType forKey:@"type"];
		[dict setObject:portCategory forKey:@"category"];
		[dict setObject:portLabel forKey:@"label"];
		[dict setObject:portImage forKey:@"image"];
		
		[freeInlets addObject:dict];
	}
	
	// Process outlets
	int expansionIndex = 0;
	int storageIndex = 0;
	int peripheralIndex = 0;
	OEPorts *outlets = info.getOutlets();
	for (OEPorts::iterator o = outlets->begin();
		 o != outlets->end();
		 o++)
	{
		if (!o->connectionPort)
			continue;
		
		NSString *deviceImage = [NSString stringWithUTF8String:o->image.c_str()];
		NSString *deviceLabel = [NSString stringWithUTF8String:o->label.c_str()];
		NSString *deviceRef = [NSString stringWithUTF8String:o->ref.getStringRef().
							   c_str()];
		
		NSImage *theImage = [self getResourceImage:deviceImage];
		NSString *text = [NSString localizedStringWithFormat:@"\n(on %@)",
						  [NSString stringWithUTF8String:
						   o->connectionLabel.c_str()]];
		NSAttributedString *theTitle = [self formatDeviceLabel:deviceLabel
										   withInformativeText:text];
		
		NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
							  theTitle, @"title",
							  theImage, @"image",
							  deviceLabel, @"label",
							  deviceRef, @"ref",
							  nil];
		
		string category = o->connectionPort->category;
		if (category == "expansion")
			[self insertObject:dict inExpansionsAtIndex:expansionIndex++];
		else if (category == "storage")
			[self insertObject:dict inStorageAtIndex:storageIndex++];
		else if (category == "peripheral")
			[self insertObject:dict inPeripheralsAtIndex:peripheralIndex++];
	}
	
	return;
}

- (BOOL) readFromURL:(NSURL *) absoluteURL
			  ofType:(NSString *) typeName
			   error:(NSError **) outError
{
	const char *emulationPath = [[absoluteURL path] UTF8String];
	const char *resourcePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
	
	if (emulation)
		oepaDestroy(emulation);
	
	emulation = oepaConstruct(emulationPath, resourcePath);
	
	if (emulation)
	{
		if (oepaIsLoaded(emulation))
		{
			[self setLabel:[self getDMLProperty:@"label"]];
			[self setNotes:[self getComponentProperty:@"notes" ref:@"host::host"]];
			[self updatePowerState];
			[self setImage:[self getResourceImage:[self getDMLProperty:@"image"]]];
			
			[self updateDevices];
			
			return YES;
		}
		
		oepaDestroy(emulation);
		emulation = NULL;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return NO;
}

- (BOOL) writeToURL:(NSURL *) absoluteURL
			 ofType:(NSString *) typeName
			  error:(NSError **) outError
{
	const char *emulationPath = [[[absoluteURL path] stringByAppendingString:@"/"]
								 UTF8String];
	if (emulation)
	{
		if (oepaSave(emulation, string(emulationPath)))
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (void) setFileModificationDate:(NSDate *) date
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

- (IBAction) saveDocumentAsTemplate:(id) sender
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

- (void) saveDocumentAsTemplateDidEnd:(NSSavePanel *) panel
						   returnCode:(int) returnCode
						  contextInfo:(void *) contextInfo
{
	if (returnCode != NSOKButton)
		return;
	
	NSError *error;
	if (![self writeToURL:[panel URL]
				   ofType:nil
					error:&error])
		[[NSAlert alertWithError:error] runModal];
}

- (void) addDevices:(NSString *) path
		connections:(NSDictionary *) connections
{
	string pathString = [path UTF8String];
	map<string, string> connectionsMap;
	
	NSEnumerator *i = [connections keyEnumerator];
	NSString *inletRef;
	
	while (inletRef = [i nextObject])
	{
		NSString *outletRef = [connections objectForKey:inletRef];
		
		string inletRefString = [inletRef UTF8String];
		string outletRefString = [outletRef UTF8String];
		
		connectionsMap[inletRefString] = outletRefString;
	}
	
	if (!oepaAddDevices(emulation, pathString, connectionsMap))
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

- (void) removeDevice:(NSDictionary *) dict
{
	NSString *deviceRef = [dict objectForKey:@"ref"];
	NSString *deviceLabel = [dict objectForKey:@"label"];
	
	string refString = [deviceRef UTF8String];
	
	if (!oepaIsDeviceTerminal(emulation, refString))
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
	}
	
	if (!oepaRemoveDevice(emulation, refString))
	{
		NSString *messageText = @"The device could not be removed.";
		
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert setMessageText:NSLocalizedString(messageText, messageText)];
		[alert runModal];
	}
	
	[self updateDevices];
	[self updateChangeCount:NSChangeDone];
}

- (void) makeWindowControllers
{
	NSWindowController *windowController;
	
	windowController = [[DocumentWindowController alloc] init];
	[self addWindowController:windowController];
	[windowController release];
}

- (BOOL) validateUserInterfaceItem:(id) item
{
	if ([item action] == @selector(copy:))
		return [self isCopyValid];
	else if ([item action] == @selector(paste:))
		return [self isPasteValid];
	else if ([item action] == @selector(startSpeaking:))
		return [self isCopyValid];
	
	return YES;
}

- (NSImage *) image
{
	return [[image retain] autorelease];
}

- (void)setImage:(NSImage *) value
{
    if (image != value)
	{
        [image release];
        image = [value copy];
    }
}

- (NSString *) label
{
	return [[label retain] autorelease];
}

- (void) setLabel:(NSString *) value
{
    if (label != value)
	{
        [label release];
        label = [value copy];
    }
}

- (NSString *) notes
{
	return [[notes retain] autorelease];
}

- (void) setNotes:(NSString *) value
{
    if (notes != value)
	{
        [notes release];
        notes = [value copy];
    }
}

- (NSString *) modificationDate
{
	return modificationDate;
}

- (void) setModificationDate:(NSString *) value
{
    if (modificationDate != value)
	{
        [modificationDate release];
        modificationDate = [value copy];
    }
}

- (NSString *) powerState
{
	return powerState;
}

- (void)setPowerState:(NSString *) value
{
    if (powerState != value)
	{
        [powerState release];
        powerState = [value copy];
    }
}

- (NSMutableArray *) freeInlets
{
	return freeInlets;
}

- (NSMutableArray *) expansions
{
	return [[expansions retain] autorelease];
}

- (void) insertObject:(id) value inExpansionsAtIndex:(NSUInteger) index
{
    [expansions insertObject:value atIndex:index];
}

- (void) removeObjectFromExpansionsAtIndex:(NSUInteger) index
{
    [expansions removeObjectAtIndex:index];
}

- (NSMutableArray *) storage
{
	return [[storage retain] autorelease];
}

- (void) insertObject:(id) value inStorageAtIndex:(NSUInteger) index
{
    [storage insertObject:value atIndex:index];
}

- (void) removeObjectFromStorageAtIndex:(NSUInteger) index
{
    [storage removeObjectAtIndex:index];
}

- (NSMutableArray *) peripherals
{
	return [[peripherals retain] autorelease];
}

- (void) insertObject:(id) value inPeripheralsAtIndex:(NSUInteger) index
{
    [peripherals insertObject:value atIndex:index];
}

- (void) removeObjectFromPeripheralsAtIndex:(NSUInteger) index
{
    [peripherals removeObjectAtIndex:index];
}

- (void) powerButtonPressedAndReleased:(id) sender
{
	[self powerButtonPressed:sender];
	[self powerButtonReleased:sender];
}

- (void) powerButtonPressed:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_POWER, true};
//	oepaIoctl(emulation, "host::audio", OE_POST_NOTIFICATION, &event);
}

- (void) powerButtonReleased:(id) sender
{
//	OEHostSystemEvent event = {HOSTAUDIO_POWER, false};
//	oepaIoctl(emulation, "host::audio", OE_POST_NOTIFICATION, &event);
}

- (void) resetButtonPressedAndReleased:(id) sender
{
	[self resetButtonPressed:sender];
	[self resetButtonReleased:sender];
}

- (void) resetButtonPressed:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_RESET, false};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (void) resetButtonReleased:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_RESET, false};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (void) pauseButtonPressedAndReleased:(id) sender
{
	[self pauseButtonPressed:sender];
	[self pauseButtonReleased:sender];
}

- (void) pauseButtonPressed:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_PAUSE, true};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (void) pauseButtonReleased:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_PAUSE, false};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (void) interruptButtonPressedAndReleased:(id) sender
{
	[self pauseButtonPressed:sender];
	[self pauseButtonReleased:sender];
}

- (void) interruptButtonPressed:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_INTERRUPT, true};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (void) interruptButtonReleased:(id) sender
{
//	OEHostSystemEvent event = {HOSTSYSTEM_INTERRUPT, true};
//	oepaIoctl(emulation, "host::system", OE_POST_NOTIFICATION, &event);
}

- (BOOL) isCopyValid
{
	// To-Do: libemulation
	return YES;
}

- (BOOL) isPasteValid
{
	return [pasteboard availableTypeFromArray:pasteboardTypes] != nil;
}

- (NSString *) documentText
{
	// To-Do: libemulation
	return @"This is a meticulously designed test of the speech synthesizing system.";  
}

- (void) copy:(id) sender
{
	if ([self isCopyValid])
	{
		[pasteboard declareTypes:pasteboardTypes owner:self];
		[pasteboard setString:[self documentText] forType:NSStringPboardType];
	}
}

- (void) paste:(id) sender
{
	if ([self isPasteValid])
	{
//		NSString *text = [pasteboard stringForType:NSStringPboardType];
		
		// To-do: Send to libemulator
		// (using [text UTF8String])
	}
}

- (void) startSpeaking:(id) sender
{
	NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
	[dummy insertText:[self documentText]];
	[dummy startSpeaking:self];
}

@end
