
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

#import "CanvasWindowController.h"

#import "OEInfo.h"
#import "OEPortAudioEmulation.h"
#import "HostInterface.h"

#import "StringConversion.h"

@implementation Document

- (id)init
{
	if (self = [super init])
	{
		emulation = nil;
		
		freeInlets = [[NSMutableArray alloc] init];
		
		devices = [[NSMutableArray alloc] init];
		
		devicesWindowController = nil;
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
	
	[devices release];
	
	[devicesWindowController release];
	
	[super dealloc];
}

- (void)showDevices:(id)sender
{
	[devicesWindowController showWindow:sender];
}

- (void)constructEmulation:(NSURL *)url
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	OEPortAudio *oePortAudio = (OEPortAudio *)[documentController getOEPortAudio];
	
	string path = getString([url path]);
	string resourcePath = getString([[NSBundle mainBundle] resourcePath]);
	
	emulation = new OEPortAudioEmulation(oePortAudio, path, resourcePath);
	
	[documentController addEmulation:emulation];
}

- (void)deleteEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	
	[documentController removeEmulation:emulation];
	
	delete (OEPortAudioEmulation *)emulation;
	
	emulation = nil;
}

- (void *)emulation
{
	return emulation;
}

- (int)postMessage:(NSString *)device
		   message:(int)message
			  data:(void *)data
{
	return ((OEPortAudioEmulation *)emulation)->postMessage(getString(device),
															message,
															data);
}

- (void)notify:(NSString *)device
  notification:(int)notification
		  data:(void *)data
{
	((OEPortAudioEmulation *)emulation)->notify(getString(device),
												notification,
												data);
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

- (void)updateDevices
{
	int count;
	
	// Clean up
	[freeInlets removeAllObjects];
	
	count = [devices count];
	for (int i = 0; i < count; i++)
		[self removeObjectFromDevicesAtIndex:0];
	
	// Process inlets
	OEPorts *inlets = ((OEPortAudioEmulation *)emulation)->getInlets();
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
	int deviceIndex = 0;
	OEDevices *oeDevices = ((OEPortAudioEmulation *)emulation)->getDevices();
	for (OEDevices::iterator device = oeDevices->begin();
		 device != oeDevices->end();
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
		
		[self insertObject:dict inDevicesAtIndex:deviceIndex++];
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
		if (((OEPortAudioEmulation *)emulation)->isOpen())
		{
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
		
		if (((OEPortAudioEmulation *)emulation)->save(emulationPath))
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
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

- (void)addEDL:(NSString *)path
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
	
	if (!((OEPortAudioEmulation *)emulation)->addEDL(pathString, connectionsMap))
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
	
	if (!((OEPortAudioEmulation *)emulation)->removeDevice(refString))
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
	devicesWindowController = [[DevicesWindowController alloc] init];
	[self addWindowController:devicesWindowController];
	
	NSWindowController *windowController;
	windowController = [[CanvasWindowController alloc] init];
	[self addWindowController:windowController];
	[windowController release];
}

- (NSMutableArray *)freeInlets
{
	return freeInlets;
}

- (NSMutableArray *)devices
{
	return [[devices retain] autorelease];
}

- (void)insertObject:(id)value inDevicesAtIndex:(NSUInteger)index
{
    [devices insertObject:value atIndex:index];
}

- (void)removeObjectFromDevicesAtIndex:(NSUInteger)index
{
    [devices removeObjectAtIndex:index];
}

@end
