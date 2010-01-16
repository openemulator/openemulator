
/**
 * OpenEmulator
 * Mac OS X Chooser View Controller
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device or template chooser view.
 */

#import <Cocoa/Cocoa.h>

@protocol ConnectorDelegate <NSObject>

@optional
- (void)connectorWasDoubleClicked:(id)sender;

@end

@interface ConnectorViewController : NSViewController
{
	id fImageBrowserView;
	
	id<ConnectorDelegate> connectorDelegate;
	
	NSArray *items;
}

- (void) setDelegate:(id)theDelegate;

@end
