
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#define TEMPLATE_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

@class DevicesWindowController;

@interface Document : NSDocument
{
	void *emulation;
	
	NSMutableArray *devices;
	NSMutableArray *freePorts;
	
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

- (NSArray *)freePorts;

- (NSArray *)devices;
- (void)insertObject:(id)value inDevicesAtIndex:(NSUInteger)index;
- (void)removeObjectFromDevicesAtIndex:(NSUInteger)index;

@end
