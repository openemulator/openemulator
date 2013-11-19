
/**
 * OpenEmulator
 * Vertically Centered Text Field Cell
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a vertically centered text field cell
 */

#import <Cocoa/Cocoa.h>

@interface VerticallyCenteredTextFieldCell : NSTextFieldCell
{
    float horizontalInset;
}

- (void)setHorizontalInset:(float)theInset;

@end
