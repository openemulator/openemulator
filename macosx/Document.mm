
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import "Document.h"
#import "DocumentController.h"
#import "EmulationWindowController.h"
#import "CanvasWindowController.h"
#import "StringConversion.h"

#import "Emulation.h"
#import "StorageInterface.h"
#import "PortAudioHAL.h"
#import "OpenGLHAL.h"

#import <sstream>

@implementation Document

void didUpdate(void *userData)
{
	Document *document = (Document *)userData;
	[document performSelectorOnMainThread:@selector(didUpdate:)
							   withObject:nil
							waitUntilDone:NO];
}

void runAlert(void *userData, string message)
{
	// There is no autorelease pool when this method is called
	// (it is called from a background thread)
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	NSString *theMessage = getNSString(message);
	[document performSelectorOnMainThread:@selector(runAlert:)
							   withObject:theMessage
							waitUntilDone:YES];
	
	[pool release];
}

OEComponent *createCanvas(void *userData, string title)
{
	// There is no autorelease pool when this method is called
	// (it is called from a background thread)
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	OpenGLHAL *canvas = new OpenGLHAL();
	NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSValue valueWithPointer:canvas], @"canvas",
						  getNSString(title), @"title",
						  nil];
	
	Document *document = (Document *)userData;
	[document performSelectorOnMainThread:@selector(createCanvas:)
							   withObject:dict
							waitUntilDone:YES];
	
	[pool release];
	
	return canvas;
}

void destroyCanvas(void *userData, OEComponent *canvas)
{
	Document *document = (Document *)userData;
	[document performSelectorOnMainThread:@selector(destroyCanvas:)
							   withObject:[NSValue valueWithPointer:canvas]
							waitUntilDone:YES];
	
	delete canvas;
}

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	if (self = [super init])
	{
		canvases = [[NSMutableArray alloc] init];
		canvasWindowControllers = [[NSMutableArray alloc] init];
		
		if ([self readFromURL:absoluteURL
					   ofType:nil
						error:outError])
			return self;
		
		[canvases release];
		[canvasWindowControllers release];
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileReadUnknownError
								userInfo:nil];
	return nil;
}

- (void)dealloc
{
	NSLog(@"Deleted document");
	[self deleteEmulation];
	
	[emulationWindowController release];
	
	[canvases release];
	[canvasWindowControllers release];
	
	[super dealloc];
}



- (void)newEmulation:(NSURL *)url
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	Emulation *theEmulation = new Emulation();
	
	theEmulation->setResourcePath(getCPPString([[NSBundle mainBundle] resourcePath]));
	
	theEmulation->setComponent("audio", portAudioHAL);
	
	theEmulation->setRunAlert(runAlert);
	theEmulation->setCreateCanvas(createCanvas);
	theEmulation->setDestroyCanvas(destroyCanvas);
	theEmulation->setUserData(self);
	
	theEmulation->open(getCPPString([url path]));
	
	theEmulation->setDidUpdate(didUpdate);
	
	portAudioHAL->addEmulation((Emulation *)emulation);
	
	emulation = theEmulation;
}

- (void)deleteEmulation
{
	if (!emulation)
		return;
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	Emulation *theEmulation = (Emulation *)emulation;
	portAudioHAL->removeEmulation(theEmulation);
	
	theEmulation->setDidUpdate(NULL);
	theEmulation->setRunAlert(NULL);
	theEmulation->setCreateCanvas(NULL);
	theEmulation->setDestroyCanvas(NULL);
	
	delete theEmulation;
	emulation = nil;
}

- (void)lockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	portAudioHAL->lockEmulations();
}

- (void)unlockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PortAudioHAL *portAudioHAL = (PortAudioHAL *)[documentController portAudioHAL];
	
	portAudioHAL->unlockEmulations();
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self deleteEmulation];
	[self newEmulation:absoluteURL];
	
	if (emulation)
	{
		if (((Emulation *)emulation)->isOpen())
			return YES;
		
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
		string emulationPath = getCPPString([[absoluteURL path]
										   stringByAppendingString:@"/"]);
		
		[self lockEmulation];
		bool isSaved = ((Emulation *)emulation)->save(emulationPath);
		[self unlockEmulation];
		
		if (isSaved)
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (IBAction)saveDocumentAsTemplate:(id)sender
{
	NSString *path = [USER_TEMPLATES_FOLDER stringByExpandingTildeInPath];
	
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

- (void)makeWindowControllers
{
	emulationWindowController = [[EmulationWindowController alloc] init];
	
	if ([canvasWindowControllers count])
	{
		CanvasWindowController *canvasWindowController;
		canvasWindowController = [canvasWindowControllers objectAtIndex:0];
		[self addWindowController:canvasWindowController];
	}
	else
		[self addWindowController:emulationWindowController];
}



- (void *)emulationInfo
{
	Emulation *theEmulation = (Emulation *)emulation;
	return theEmulation->getEmulationInfo();
}

- (IBAction)showEmulation:(id)sender
{
	if (![[self windowControllers] containsObject:emulationWindowController])
		[self addWindowController:emulationWindowController];
	
	[emulationWindowController showWindow:self];
}

- (IBAction)didUpdate:(id)sender
{
	[emulationWindowController updateEmulation:sender];
}

- (void)runAlert:(NSString *)string
{
	NSArray *lines = [string componentsSeparatedByString:@"\n"];
	NSString *messageText = [lines objectAtIndex:0];
	NSString *informativeText = @"";
	
	if ([lines count] > 1)
		informativeText = [lines objectAtIndex:1];
	
	NSRunAlertPanel(messageText, informativeText, nil, nil, nil);
}

- (void)createCanvas:(NSDictionary *)dict
{
	OpenGLHAL *canvas = (OpenGLHAL *)[[dict objectForKey:@"canvas"] pointerValue];
	NSString *title = [dict objectForKey:@"title"];
	
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [[CanvasWindowController alloc] initWithTitle:title
																	canvas:canvas];
	
	[canvases addObject:[NSValue valueWithPointer:canvas]];
	[canvasWindowControllers addObject:canvasWindowController];
	
	[canvasWindowController release];
}

- (void)destroyCanvas:(NSValue *)canvas
{
	NSInteger index = [canvases indexOfObject:[NSValue valueWithPointer:canvas]];
	
	[self removeWindowController:[canvasWindowControllers objectAtIndex:index]];
	
	[canvases removeObjectAtIndex:index];
	[canvasWindowControllers removeObjectAtIndex:index];
}

- (void)showCanvas:(void *)canvas
{
	NSInteger index = [canvases indexOfObject:[NSValue valueWithPointer:canvas]];
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [canvasWindowControllers objectAtIndex:index];
	
	if (![[self windowControllers] containsObject:canvasWindowController])
		[self addWindowController:canvasWindowController];
	
	[canvasWindowController showWindow:self];
}

- (BOOL)getBoolForMessage:(int)message
				component:(void *)component
{
	BOOL result = NO;
	
	if (component)
	{
		[self lockEmulation];
		result = ((OEComponent *)component)->postMessage(NULL, message, NULL);
		[self unlockEmulation];
	}
	
	return result;
}

- (OEUInt64)getOEUInt64ForMessage:(int)message
						component:(void *)component
{
	OEUInt64 value = 0;
	
	if (component)
	{
		[self lockEmulation];
		((OEComponent *)component)->postMessage(NULL, message, &value);
		[self unlockEmulation];
	}
	
	return value;
}

- (BOOL)postString:(NSString *)aString
		   message:(int)message
		 component:(void *)component
{
	string theString = getCPPString(aString);
	BOOL result = NO;
	
	if (component)
	{
		[self lockEmulation];
		result = ((OEComponent *)component)->postMessage(NULL, message, &theString);
		[self unlockEmulation];
	}
	
	return result;
}

- (NSString *)getStringForMessage:(int)message
						component:(void *)component
{
	string value;
	
	if (component)
	{
		[self lockEmulation];
		((OEComponent *)component)->postMessage(NULL, message, &value);
		[self unlockEmulation];
	}
	
	return getNSString(value);
}

- (NSString *)formatCapacity:(OEUInt64)value
{
	if (value < (1 << 20))
		return [NSString stringWithFormat:@"%3.0f kiB", value / (1 << 10)];
	else if (value < (1 << 30))
		return [NSString stringWithFormat:@"%3.2f MiB", value / (1 << 20)];
	else
		return [NSString stringWithFormat:@"%3.2f GiB", value / (1 << 30)];
}

- (BOOL)mount:(NSString *)path
{
	Emulation *theEmulation = (Emulation *)emulation;
	EmulationInfo *theEmulationInfo = theEmulation->getEmulationInfo();
	for (int i = 0; i < theEmulationInfo->size(); i++)
	{
		EmulationDeviceInfo &deviceInfo = theEmulationInfo->at(i);
		if (deviceInfo.storage)
		{
			if ([self mount:path inStorage:deviceInfo.storage])
				return YES;
		}
	}
	
	return NO;
}

- (BOOL)mount:(NSString *)path inStorage:(void *)component
{
	if ([self getBoolForMessage:(int)STORAGE_IS_LOCKED
					  component:component])
		return NO;
	
	return [self postString:path
					message:(int)STORAGE_MOUNT
				  component:component];
}

- (BOOL)unmountStorage:(void *)component
{
	return [self postString:@""
					message:(int)STORAGE_UNMOUNT
				  component:component];
}

- (BOOL)canMount:(NSString *)path
{
	Emulation *theEmulation = (Emulation *)emulation;
	EmulationInfo *theEmulationInfo = theEmulation->getEmulationInfo();
	for (int i = 0; i < theEmulationInfo->size(); i++)
	{
		EmulationDeviceInfo &deviceInfo = theEmulationInfo->at(i);
		if (deviceInfo.storage)
		{
			if ([self canMount:path inStorage:deviceInfo.storage])
				return YES;
		}
	}
	
	return NO;
}

- (BOOL)canMount:(NSString *)path inStorage:(void *)component
{
	if ([self getBoolForMessage:(int)STORAGE_IS_LOCKED component:component])
		return NO;
	
	return [self getBoolForMessage:(int)STORAGE_IS_IMAGE_SUPPORTED
						 component:component];
}

- (BOOL)isImageSupported:(NSString *)path
{
	Emulation *theEmulation = (Emulation *)emulation;
	EmulationInfo *theEmulationInfo = theEmulation->getEmulationInfo();
	for (int i = 0; i < theEmulationInfo->size(); i++)
	{
		EmulationDeviceInfo &deviceInfo = theEmulationInfo->at(i);
		if (deviceInfo.storage)
		{
			if ([self getBoolForMessage:(int)STORAGE_IS_IMAGE_SUPPORTED
							  component:deviceInfo.storage])
				return YES;
		}
	}
	
	return NO;
}

- (BOOL)isStorageMounted:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_MOUNTED
						 component:component];
}

- (BOOL)isStorageWritable:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_WRITABLE
						 component:component];
}

- (BOOL)isStorageLocked:(void *)component
{
	return [self getBoolForMessage:(int)STORAGE_IS_LOCKED
						 component:component];
}

- (NSString *)imagePathForStorage:(void *)component
{
	return [self getStringForMessage:(int)STORAGE_GET_IMAGE_PATH
						   component:component];
}

- (NSString *)imageFormatForStorage:(void *)component
{
	return [self getStringForMessage:(int)STORAGE_GET_IMAGE_FORMAT
						   component:component];
}

- (NSString *)imageCapacityForStorage:(void *)component
{
	OEUInt64 value = [self getOEUInt64ForMessage:(int)STORAGE_GET_IMAGE_CAPACITY
									   component:component];
	return [self formatCapacity:value];
}



- (void)setValue:(NSString *)theValue
	  ofProperty:(NSString *)theName
	forComponent:(NSString *)theId
{
	if (!emulation)
		return;
	
	Emulation *theEmulation = (Emulation *)emulation;
	BOOL success = NO;
	
	[self lockEmulation];
	OEComponent *component = theEmulation->getComponent(getCPPString(theId));
	if (component)
		success = component->setValue(getCPPString(theName), getCPPString(theValue));
	[self unlockEmulation];
	
	if (!success)
		NSLog(@"could not set property '%@' for '%@'", theName, theId);
}

- (NSString *)valueOfProperty:(NSString *)theName
				 forComponent:(NSString *)theId
{
	if (!emulation)
		return @"";
	
	Emulation *theEmulation = (Emulation *)emulation;
	BOOL success = NO;
	string value;
	
	[self lockEmulation];
	OEComponent *component = theEmulation->getComponent(getCPPString(theId));
	if (component)
		success = component->getValue(getCPPString(theName), value);
	[self unlockEmulation];
	
	if (!success)
		NSLog(@"invalid property '%@' for '%@'", theName, theId);
	
	return getNSString(value);
}



- (void)addEDL:(NSString *)path
   connections:(NSDictionary *)connections
{
	/*	if (!emulation)
	 return;
	 
	 string pathString = getCPPString(path);
	 map<string, string> connectionsMap;
	 
	 NSEnumerator *i = [connections keyEnumerator];
	 NSString *inletRef;
	 
	 while (inletRef = [i nextObject])
	 {
	 NSString *outletRef = [connections objectForKey:inletRef];
	 
	 string inletRefString = getCPPString(inletRef);
	 string outletRefString = getCPPString(outletRef);
	 
	 connectionsMap[inletRefString] = outletRefString;
	 }
	 
	 [self lockEmulation];
	 bool isAdded = ((Emulation *)emulation)->addEDL(pathString, connectionsMap);
	 [self unlockEmulation];
	 
	 if (!isAdded)
	 {
	 NSString *messageText = @"The device could not be added.";
	 
	 NSAlert *alert = [[NSAlert alloc] init];
	 [alert setMessageText:NSLocalizedString(messageText, messageText)];
	 [alert setAlertStyle:NSWarningAlertStyle];
	 [alert runModal];
	 [alert release];
	 }
	 
	 [self updateChangeCount:NSChangeDone];*/
}

- (void)removeDevice:(NSString *)deviceId
{
	/*	if (!emulation)
	 return;
	 
	 //	NSString *deviceRef = [dict objectForKey:@"ref"];
	 //	NSString *deviceLabel = [dict objectForKey:@"label"];
	 
	 //	string refString = getCPPString(deviceRef);
	 
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
	 }
	 
	 [self lockEmulation];
	 bool isRemoved = ((OEEmulation *)emulation)->removeDevice(refString);
	 [self unlockEmulation];
	 
	 if (!isRemoved)
	 {
	 NSString *messageText = @"The device could not be removed.";
	 
	 NSAlert *alert = [[NSAlert alloc] init];
	 [alert setMessageText:NSLocalizedString(messageText, messageText)];
	 [alert runModal];
	 [alert release];
	 }
	 
	 [self updateChangeCount:NSChangeDone];*/
}

@end
