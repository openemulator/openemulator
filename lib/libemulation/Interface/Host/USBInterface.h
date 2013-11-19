
/**
 * libemulation
 * USB interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the USB interface
 */

#ifndef _USBINTERFACE_H
#define _USBINTERFACE_H

typedef enum
{
    USB_GET_DEVICELIST,
    USB_OPEN_DEVICE,
    USB_CLOSE_DEVICE,
    USB_SUBMIT_TRANSFER,
    USB_CANCEL_TRANSFER,
} USBMessage;

#endif
