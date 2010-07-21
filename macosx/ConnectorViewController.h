
/**
 * OpenEmulator
 * Mac OS X Connector View Controller
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a connector view.
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
	
	NSMutableArray *items;
}

- (void)setDelegate:(id)theDelegate;
- (void)updateWithInlets:(NSArray *)inlets;
- (NSString *)selectedInletRef;

@end
