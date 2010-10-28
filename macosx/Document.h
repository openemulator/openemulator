
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#import "DevicesWindowController.h"

#define TEMPLATE_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

@interface Document : NSDocument
{
	void *emulation;
	
	NSMutableArray *freeInlets;
	
	NSMutableArray *devices;
	
	DevicesWindowController *devicesWindowController;
}

- (void)showDevices:(id)sender;

- (void)constructEmulation:(NSURL *)url;
- (void)deleteEmulation;
- (void *)emulation;

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;

- (void)addEDL:(NSString *)path connections:(NSDictionary *)connections;
- (void)removeDevice:(NSDictionary *)dict;

- (NSMutableArray *)freeInlets;

- (NSMutableArray *)devices;
- (void)insertObject:(id)value inDevicesAtIndex:(NSUInteger)index;
- (void)removeObjectFromDevicesAtIndex:(NSUInteger)index;

@end
