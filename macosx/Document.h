
/**
 * OpenEmulator
 * Mac OS X Document
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Handles an emulation.
 */

#import <Cocoa/Cocoa.h>

#define USER_TEMPLATES_FOLDER @"~/Library/Application Support/OpenEmulator/Templates"

@class EmulationWindowController;

@interface Document : NSDocument
{
	void *emulation;
	void *devicesInfo;
	
	EmulationWindowController *emulationWindowController;
}

- (void)showEmulation:(id)sender;

- (void)newEmulation:(NSURL *)url;
- (void)deleteEmulation;

- (id)initWithTemplateURL:(NSURL *)templateURL error:(NSError **)outError;
- (IBAction)saveDocumentAsTemplate:(id)sender;

- (void *)devicesInfo;

- (NSString *)getEDLOptions;
- (void)setEDLOptions:(NSString *)value;

- (void)addEDL:(NSString *)path connections:(NSDictionary *)connections;
- (void)removeDevice:(NSString *)deviceId;

- (NSString *)getValueOfProperty:(NSString *)name
					   component:(NSString *)theId;
- (void)setValue:(NSString *)value
	  ofProperty:(NSString *)theName
	   component:(NSString *)theId;
- (BOOL)mount:(NSString *)path;
- (BOOL)mount:(NSString *)path component:(NSString *)deviceId;
- (BOOL)mountable:(NSString *)path;

@end
