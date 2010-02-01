
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
		
		power = false;
		label = nil;
		description = nil;
		modificationDate = nil;
		runTime = nil;
		image = nil;
		
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
//	printf("initWithemplateURL\n");
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
//	printf("dealloc\n");
	if (emulation)
		delete (OEEmulation *) emulation;
	
	[pasteboardTypes release];
	
	[freeInlets release];
	
	[expansions release];
	[storage release];
	[peripherals release];
	
	[super dealloc];
}

- (void) setDMLProperty:(NSString *) name value:(NSString *) value
{
	if (!emulation)
		return;
	
	xmlDocPtr dml = ((OEEmulation *) emulation)->getDML();
	
	xmlNodePtr rootNode = xmlDocGetRootElement(dml);
	
	xmlSetProp(rootNode, BAD_CAST [name UTF8String], BAD_CAST [value UTF8String]);
}

- (NSString *) getDMLProperty:(NSString *) name
{
	if (!emulation)
		return nil;
	
	xmlDocPtr dml = ((OEEmulation *) emulation)->getDML();
	
	xmlNodePtr rootNode = xmlDocGetRootElement(dml);
	
	xmlChar *valuec = xmlGetProp(rootNode, BAD_CAST [name UTF8String]);
	NSString *value = [NSString stringWithUTF8String:(const char *) valuec];
	xmlFree(valuec);
	
	return value;
}

- (void) setIoctlProperty:(NSString *) name ref:(NSString *) ref value:(NSString *) value
{
	if (!emulation)
		return;
	
	OEIoctlProperty property;
	
	property.name = string([name UTF8String]);
	property.value = string([value UTF8String]);
	
	((OEEmulation *)emulation)->ioctl(string([ref UTF8String]),
									  OEIoctlSetProperty,
									  &property);
}

- (NSString *) getIoctlProperty:(NSString *)name ref:(NSString *) ref
{
	if (!emulation)
		return nil;
	
	OEIoctlProperty msg;
	
	msg.name = string([name UTF8String]);
	
	if (((OEEmulation *)emulation)->ioctl(string([ref UTF8String]),
									  OEIoctlGetProperty,
									  &msg))
		return [NSString stringWithUTF8String:msg.value.c_str()];
	else
		return nil;
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

- (void) updateRunTime
{
	NSString *property = [self getIoctlProperty:@"runTime" ref:@"host::events"];
	int timeDifference = [property intValue];
	
	int seconds = timeDifference % 60;
	int minutes = (timeDifference / 60) % 60; 
	int hours = (timeDifference / 3600) % 3600;
	
	NSString *value = [NSString stringWithFormat:@"%d:%02d:%02d",
					   hours,  minutes, seconds];
	
	[self setRunTime:value];
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

- (void) updateDevices
{
	[freeInlets release];
	freeInlets = [[NSMutableArray alloc] init];
	
	[expansions release];
	expansions = [[NSMutableArray alloc] init];
	[storage release];
	storage = [[NSMutableArray alloc] init];
	[peripherals release];
	peripherals = [[NSMutableArray alloc] init];
	
	// Process info
	xmlDocPtr dmlDocPtr = ((OEEmulation *) emulation)->getDML();
	OEInfo info(dmlDocPtr);
	if (!info.isLoaded())
		return;
	
	// Build free inlets array
	OEPorts *inlets = info.getInlets();
	for (OEPorts::iterator i = inlets->begin();
		 i != inlets->end();
		 i++)
	{
		if (i->connectionPort)
			continue;
		
		string stringRef = i->ref.getStringRef();
		
		NSString *portType = [NSString stringWithUTF8String:i->type.c_str()];
		NSString *portLabel = [NSString stringWithUTF8String:i->connectionLabel.c_str()];
		NSString *portImage = [NSString stringWithUTF8String:i->image.c_str()];
		NSString *portRef = [NSString stringWithUTF8String:stringRef.c_str()];
		
		NSMutableDictionary *dict = [[[NSMutableDictionary alloc] init] autorelease];
		[dict setObject:portType forKey:@"type"];
		[dict setObject:portLabel forKey:@"label"];
		[dict setObject:portImage forKey:@"image"];
		[dict setObject:portRef forKey:@"ref"];
		
		[freeInlets addObject:dict];
	}
	
	// Build outlet lists
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
		
		NSString *imagePath = [NSString stringWithUTF8String:o->image.c_str()];
		NSString *deviceLabel = [NSString stringWithUTF8String:o->label.c_str()];
		
		NSImage *deviceImage = [self getResourceImage:imagePath];
		NSString *informativeText = [NSString localizedStringWithFormat:@"\n(on %@)",
									 [NSString stringWithUTF8String:
									  o->connectionLabel.c_str()]];
		NSAttributedString *aString = [self formatDeviceLabel:deviceLabel
										  withInformativeText:informativeText];
		
		NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
							  aString, @"title",
							  deviceImage, @"image",
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
//	printf("readFromURL\n");
	const char *emulationPath = [[absoluteURL path] UTF8String];
	const char *resourcePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
	
	if (emulation)
		delete (OEEmulation *)emulation;
	
	emulation = (void *)new OEEmulation(emulationPath, resourcePath);
	
	if (emulation)
	{
		if (((OEEmulation *)emulation)->isOpen())
		{
			[self setLabel:[self getDMLProperty:@"label"]];
			[self setGroup:[self getDMLProperty:@"group"]];
			[self setDescription:[self getDMLProperty:@"description"]];
			[self updateRunTime];
			[self setImage:[self getResourceImage:[self getDMLProperty:@"image"]]];
			
			[self updateDevices];
			
			return YES;
		}
		
		delete (OEEmulation *)emulation;
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
//	printf("writeToURL\n");
	const char *emulationPath = [[[absoluteURL path] stringByAppendingString:@"/"]
								 UTF8String];
	if (emulation)
	{
		if (((OEEmulation *)emulation)->save(string(emulationPath)))
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
	
	((OEEmulation *) emulation)->addDevices(pathString, connectionsMap);
	
	[self updateDevices];
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

- (void) powerButtonPressedAndReleased:(id) sender
{
	[self powerButtonPressed:sender];
	[self powerButtonReleased:sender];
}

- (void) powerButtonPressed:(id) sender
{
	// To-Do: libemulation
	[self setPower:![self power]];
}

- (void) powerButtonReleased:(id) sender
{
	// To-Do: libemulation
}

- (void) resetButtonPressedAndReleased:(id) sender
{
	[self resetButtonPressed:sender];
	[self resetButtonReleased:sender];
}

- (void) resetButtonPressed:(id) sender
{
	// To-Do: libemulation
}

- (void) resetButtonReleased:(id) sender
{
	// To-Do: libemulation
}

- (void) pauseButtonPressedAndReleased:(id) sender
{
	[self pauseButtonPressed:sender];
	[self pauseButtonReleased:sender];
}

- (void) pauseButtonPressed:(id) sender
{
	// To-Do: libemulation
}

- (void) pauseButtonReleased:(id) sender
{
	// To-Do: libemulation
}

- (BOOL) isCopyValid
{
	return YES; // To-Do: libemulation
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
	NSTextView *dummy = [[NSTextView alloc] init];
	[dummy insertText:[self documentText]];
	[dummy startSpeaking:self];
	[dummy release];
}

- (BOOL) power
{
	return power;
}

- (void) setPower:(BOOL) value
{
	if (power != value)
		power = value;
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

- (NSString *) group
{
	return [[group retain] autorelease];
}

- (void) setGroup:(NSString *) value
{
    if (group != value)
	{
        [group release];
        group = [value copy];
    }
}

- (NSString *) description
{
	return [[description retain] autorelease];
}

- (void) setDescription:(NSString *) value
{
    if (description != value)
	{
		if (description)
			[self updateChangeCount:NSChangeDone];
		
		[self setDMLProperty:@"description" value:value];
		
        [description release];
        description = [value copy];
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

- (NSString *) runTime
{
	return runTime;
}

- (void)setRunTime:(NSString *) value
{
    if (runTime != value)
	{
        [runTime release];
        runTime = [value copy];
    }
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

@end
