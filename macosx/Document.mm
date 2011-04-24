
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

#import "OEEmulation.h"
#import "PAAudio.h"
#import "OpenGLCanvas.h"

#import "DeviceInterface.h"
#import "StorageInterface.h"

#import <sstream>

@implementation Document

// Callback methods

void didUpdate(void *userData)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	
	[document performSelectorOnMainThread:@selector(didUpdate:)
							   withObject:nil
							waitUntilDone:NO];
	
	[pool release];
}

void runAlert(void *userData, string message)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	NSString *theMessage = getNSString(message);
	
	[document performSelectorOnMainThread:@selector(runAlert:)
							   withObject:theMessage
							waitUntilDone:NO];
	
	[pool release];
}

OEComponent *createCanvas(void *userData, OEComponent *device)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	OpenGLCanvas *canvas = new OpenGLCanvas(getCPPString([[NSBundle mainBundle] resourcePath]));
	string label;
	device->postMessage(NULL, DEVICE_GET_LABEL, &label);
	
	Document *document = (Document *)userData;
	NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSValue valueWithPointer:device], @"device",
						  getNSString(label), @"label",
						  [NSValue valueWithPointer:canvas], @"canvas",
						  nil];
	
	[document performSelectorOnMainThread:@selector(createCanvas:)
							   withObject:dict
							waitUntilDone:YES];
	
	[pool release];
	
	return canvas;
}

void destroyCanvas(void *userData, OEComponent *canvas)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	Document *document = (Document *)userData;
	NSValue *canvasValue = [NSValue valueWithPointer:canvas];
	
	[document performSelectorOnMainThread:@selector(destroyCanvas:)
							   withObject:canvasValue
							waitUntilDone:YES];
	
	delete canvas;
	
	[pool release];
}

// Alloc/init/dealloc

- (id)initWithTemplateURL:(NSURL *)absoluteURL
					error:(NSError **)outError
{
	NSLog(@"Document init");
	
	if (self = [super init])
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
	NSLog(@"Document dealloc");
	
	[self destroyEmulation];
	
	[emulationWindowController release];
	[canvasWindowControllers release];
	[canvases release];
	
	[super dealloc];
}

// Document UI

- (NSWindow *)windowForSheet
{
	return [NSApp mainWindow];
}

- (BOOL)readFromURL:(NSURL *)absoluteURL
			 ofType:(NSString *)typeName
			  error:(NSError **)outError
{
	[self destroyEmulation];
	[self createEmulation:absoluteURL];
	
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation)
	{
		if (theEmulation->isOpen())
		{
			if (theEmulation->isActive())
				[self updateChangeCount:NSChangeDone];
			
			return YES;
		}
		
		[self destroyEmulation];
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
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation)
	{
		string emulationPath = getCPPString([[absoluteURL path]
											 stringByAppendingString:@"/"]);
		
		[self lockEmulation];
		bool isSaved = theEmulation->save(emulationPath);
		[self unlockEmulation];
		
		if (isSaved)
			return YES;
	}
	
	*outError = [NSError errorWithDomain:NSCocoaErrorDomain
									code:NSFileWriteUnknownError
								userInfo:nil];
	return NO;
}

- (BOOL)saveToURL:(NSURL *)absoluteURL
		   ofType:(NSString *)typeName
 forSaveOperation:(NSSaveOperationType)saveOperation
			error:(NSError **)outError
{
	BOOL result = [super saveToURL:absoluteURL
							ofType:typeName
				  forSaveOperation:saveOperation
							 error:outError];
	
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	if (theEmulation->isActive())
		[self updateChangeCount:NSChangeDone];
	
	return result;
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
	NSLog(@"Document makeWindowControllers");
	
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

- (IBAction)showEmulation:(id)sender
{
	if (![[self windowControllers] containsObject:emulationWindowController])
		[self addWindowController:emulationWindowController];
	
	[emulationWindowController showWindow:self];
}

- (void)didUpdate:(id)sender
{
	if (emulation && ((OEEmulation *)emulation)->isActive())
		[self updateChangeCount:NSChangeDone];
	
	[emulationWindowController updateEmulation:self];
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
	NSLog(@"Document createCanvas");
	
	void *device = [[dict objectForKey:@"device"] pointerValue];
	NSString *label = [dict objectForKey:@"label"];
	OpenGLCanvas *canvas = (OpenGLCanvas *)[[dict objectForKey:@"canvas"] pointerValue];
	
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [[CanvasWindowController alloc] initWithDevice:device
																	  title:label
																	 canvas:canvas];
	
	[canvases addObject:[NSValue valueWithPointer:canvas]];
	[canvasWindowControllers addObject:canvasWindowController];
	
	[canvasWindowController release];
}

- (void)destroyCanvas:(NSValue *)canvasValue
{
	NSLog(@"Document destroyCanvas");
	
	NSInteger index = [canvases indexOfObject:canvasValue];
	
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [canvasWindowControllers objectAtIndex:index];
	
	[canvasWindowController destroyCanvas];
	
	[self removeWindowController:canvasWindowController];
	[canvases removeObjectAtIndex:index];
	[canvasWindowControllers removeObjectAtIndex:index];
}

- (void)showCanvas:(void *)canvas
{
	NSLog(@"Document showCanvas");
	
	NSInteger index = [canvases indexOfObject:[NSValue valueWithPointer:canvas]];
	CanvasWindowController *canvasWindowController;
	canvasWindowController = [canvasWindowControllers objectAtIndex:index];
	
	if (![[self windowControllers] containsObject:canvasWindowController])
		[self addWindowController:canvasWindowController];
	
	[canvasWindowController showWindow:self];
}

- (NSPrintOperation *)printOperationWithSettings:(NSDictionary *)printSettings
										   error:(NSError **)outError
{
	NSLog(@"Document printOperationWithSettings");
	
	return [super printOperationWithSettings:printSettings
									   error:outError];
}

// Emulation

- (void)createEmulation:(NSURL *)url
{
	NSLog(@"Document createEmulation");
	
	if (!canvases)
		canvases = [[NSMutableArray alloc] init];
	if (!canvasWindowControllers)
		canvasWindowControllers = [[NSMutableArray alloc] init];
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	OEEmulation *theEmulation = new OEEmulation();
	
	theEmulation->setResourcePath(getCPPString([[NSBundle mainBundle] resourcePath]));
	theEmulation->setRunAlert(runAlert);
	theEmulation->setCreateCanvas(createCanvas);
	theEmulation->setDestroyCanvas(destroyCanvas);
	theEmulation->setUserData(self);
	
	theEmulation->addComponent("audio", paAudio);
	
	theEmulation->open(getCPPString([url path]));
	
	theEmulation->setDidUpdate(didUpdate);
	
	emulation = theEmulation;
	
	paAudio->addEmulation(theEmulation);
}

- (void)destroyEmulation
{
	NSLog(@"Document destroyEmulation");
	
	if (!emulation)
		return;
	
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	OEEmulation *theEmulation = (OEEmulation *)emulation;
	paAudio->removeEmulation(theEmulation);
	
	delete theEmulation;
	emulation = nil;
	
	// Just in case somebody forgot to delete a canvas
	while ([canvases count])
		[self destroyCanvas:[canvases lastObject]];
}

- (void)lockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	paAudio->lockEmulations();
}

- (void)unlockEmulation
{
	DocumentController *documentController;
	documentController = [NSDocumentController sharedDocumentController];
	PAAudio *paAudio = (PAAudio *)[documentController paAudio];
	
	paAudio->unlockEmulations();
}

- (void *)emulation
{
	return emulation;
}

// Storage

- (BOOL)isMountable:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponent *storage = NULL;
		
		device->postMessage(NULL, DEVICE_GET_STORAGE, &storage);
		if (storage)
		{
			if (storage->postMessage(NULL, STORAGE_IS_AVAILABLE, NULL))
			{
				string thePath = getCPPString(path);
				if (storage->postMessage(NULL, STORAGE_TESTMOUNT, &thePath))
				{
					[self updateChangeCount:NSChangeDone];
					
					success = YES;
					break;
				}
			}
		}
	}
	
	[self unlockEmulation];
	
	return success;
}

- (BOOL)mount:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponent *storage = NULL;
		
		device->postMessage(NULL, DEVICE_GET_STORAGE, &storage);
		if (storage)
		{
			if (storage->postMessage(NULL, STORAGE_IS_AVAILABLE, NULL))
			{
				string thePath = getCPPString(path);
				if (storage->postMessage(NULL, STORAGE_MOUNT, &thePath))
				{
					[self updateChangeCount:NSChangeDone];
					
					success = YES;
					break;
				}
			}
		}
	}
	
	[self unlockEmulation];
	
	return success;
}

- (BOOL)testMount:(NSString *)path
{
	BOOL success = NO;
	
	[self lockEmulation];
	
	OEIds *devices = (OEIds *)((OEEmulation *)emulation)->getDevices();
	for (OEIds::iterator i = devices->begin();
		 i != devices->end();
		 i++)
	{
		OEComponent *device = ((OEEmulation *)emulation)->getComponent(*i);
		OEComponent *storage = NULL;
		
		device->postMessage(NULL, DEVICE_GET_STORAGE, &storage);
		if (storage)
		{
			string thePath = getCPPString(path);
			if (storage->postMessage(NULL, STORAGE_TESTMOUNT, NULL))
			{
				success = YES;
				break;
			}
		}
	}
	
	[self unlockEmulation];
	
	return success;
}

@end
