
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#define USER_TEMPLATES_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

typedef enum
{
	DOCUMENT_POWERDOWN,
	DOCUMENT_SLEEP,
	DOCUMENT_WAKEUP,
	DOCUMENT_COLDRESTART,
	DOCUMENT_WARMRESTART,
	DOCUMENT_DEBUGGERBREAK,
} DocumentSystemEvent;

@class EmulationWindowController;

@interface Document : NSDocument
{
	void *emulation;
	
	EmulationWindowController *emulationWindowController;
	
	NSMutableArray *canvases;
	NSMutableArray *canvasWindowControllers;
}

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;

- (void)showEmulation:(id)sender;
- (void)showCanvas:(void *)canvas;

- (void)createEmulation:(NSURL *)url;
- (void)destroyEmulation;
- (void)lockEmulation;
- (void)unlockEmulation;

- (void)setValue:(NSString *)value
	  ofProperty:(NSString *)theName
	forComponent:(NSString *)theId;
- (NSString *)valueOfProperty:(NSString *)name
				 forComponent:(NSString *)theId;

- (BOOL)sendSystemEvent:(DocumentSystemEvent)systemEvent toDevice:(void *)device;

- (BOOL)isMountPermitted:(NSString *)path;
- (BOOL)mount:(NSString *)path;
- (BOOL)isMountPossible:(NSString *)path;

- (BOOL)mount:(NSString *)path inStorage:(void *)storage;
- (BOOL)isMountPossible:(NSString *)path inStorage:(void *)storage;
- (BOOL)unmount:(void *)storage;
- (NSString *)imagePathForStorage:(void *)storage;
- (NSString *)stateLabelForStorage:(void *)storage;

@end
