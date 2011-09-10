
/**
 * libemulation
 * Ethernet interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Ethernet interface
 */

#ifndef _ETHERNETINTERFACE_H
#define _ETHERNETINTERFACE_H

typedef enum
{
    ETHERNET_SEND,
} EthernetMessage;

typedef enum
{
    ETHERNET_DATA_WAS_RECEIVED,
} EthernetNotification;

#endif
