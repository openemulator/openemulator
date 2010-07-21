
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
	
	string path = string([[url path] UTF8String]);
	string resourcePath = string([[[NSBundle mainBundle] resourcePath] UTF8String]);
	
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

- (NSString *)getDMLProperty:(NSString *)name
{
	if (!emulation)
		return nil;
	
	xmlDocPtr dml = ((OEPAEmulation *)emulation)->getDML();
	
	xmlNodePtr rootNode = xmlDocGetRootElement(dml);
	
	xmlChar *valuec = xmlGetProp(rootNode, BAD_CAST [name UTF8String]);
	NSString *value = [NSString stringWithUTF8String:(const char *)valuec];
	xmlFree(valuec);
	
	return value;
}

- (BOOL)setHostProperty:(NSString *)name
				  value:(NSString *)value
{
	if (!emulation)
		return false;
	
	return ((OEPAEmulation *)emulation)->setProperty(HOST_DEVICE,
													 string([name UTF8String]),
													 string([value UTF8String]));
}

- (NSString *)getHostProperty:(NSString *)name
{
	if (!emulation)
		return nil;
	
	string value;
	if (((OEPAEmulation *)emulation)->getProperty(HOST_DEVICE,
												  string([name UTF8String]),
												  value))
		return [NSString stringWithUTF8String:value.c_str()];
	
	return @"";
}

- (void)postHostNotification:(int)notification data:(void *)data
{
	((OEPAEmulation *)emulation)->postNotification(HOST_DEVICE, notification, data);
}

- (int)hostIoctl:(int)message data:(void *)data
{
	return ((OEPAEmulation *)emulation)->ioctl(HOST_DEVICE, message, data);
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
	xmlDocPtr dmlDocPtr = ((OEPAEmulation *)emulation)->getDML();
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

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self deleteEmulation];
	[self constructEmulation:absoluteURL];
	
	if (emulation)
	{
		if (((OEPAEmulation *)emulation)->isLoaded())
		{
			[self setLabel:[self getDMLProperty:@"label"]];
			[self setNotes:[self getHostProperty:@"notes"]];
			[self updatePowerState];
			[self setImage:[self getResourceImage:[self getDMLProperty:@"image"]]];
			
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
	const char *emulationPath = [[[absoluteURL path] stringByAppendingString:@"/"]
								 UTF8String];
	if (emulation)
	{
		[self setHostProperty:@"notes" value:[self notes]];
		
		if (((OEPAEmulation *)emulation)->save(string(emulationPath)))
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
	
	if (!((OEPAEmulation *)emulation)->addDevices(pathString, connectionsMap))
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
	
	string refString = [deviceRef UTF8String];
	
	if (!((OEPAEmulation *)emulation)->isDeviceTerminal(refString))
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

- (BOOL)isCopyable
{
	return [self hostIoctl:HOST_IS_COPYABLE data:NULL] ? YES : NO;
}

- (BOOL)isPasteable
{
	if (![self hostIoctl:HOST_IS_PASTEABLE data:NULL])
		return NO;
	
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
	
	return [pasteboard availableTypeFromArray:pasteboardTypes] != nil;
}

- (BOOL)validateUserInterfaceItem:(id)item
{
	if ([item action] == @selector(copy:))
		return [self isCopyable];
	else if ([item action] == @selector(paste:))
		return [self isPasteable];
	else if ([item action] == @selector(startSpeaking:))
		return [self isCopyable];
	
	return YES;
}

- (NSString *)documentText
{
	string characterString;
	[self postHostNotification:HOST_CLIPBOARD_COPY_EVENT data:&characterString];

	return [NSString stringWithUTF8String:characterString.c_str()];
}

- (void)copy:(id)sender
{
	if ([self isCopyable])
	{
		NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
		NSArray *pasteboardTypes = [NSArray arrayWithObjects:NSStringPboardType, nil];
		
		[pasteboard declareTypes:pasteboardTypes owner:self];
		[pasteboard setString:[self documentText] forType:NSStringPboardType];
	}
}

- (void)paste:(id)sender
{
	if ([self isPasteable])
	{
		NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
		
		NSString *characters = [pasteboard stringForType:NSStringPboardType];
		string characterString([characters UTF8String]);
		
		[self postHostNotification:HOST_CLIPBOARD_PASTE_EVENT data:&characterString];
	}
}

- (void)startSpeaking:(id)sender
{
	NSTextView *dummy = [[[NSTextView alloc] init] autorelease];
	[dummy insertText:[self documentText]];
	[dummy startSpeaking:self];
}

@end
