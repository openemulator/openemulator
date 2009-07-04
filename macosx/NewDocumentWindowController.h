
/**
 * OpenEmulator
 * Mac OS X New Document Window Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the new document window.
 */

#import <Cocoa/Cocoa.h>
#import "DocumentController.h"

@interface NewDocumentWindowController : NSWindowController {
	DocumentController *documentController;
}

- (id)init:(DocumentController*)theDocumentController;
- (void)performChoose:(id)sender;

@end
