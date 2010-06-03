/*
 *  HostHID.cpp
 *  OpenEmulator
 *
 *  Created by Marc S. Reßl on 25/05/10.
 *  Copyright 2010 ITBA. All rights reserved.
 *
 */

#include "HostHID.h"

HostHID::HostHID()
{
	hostObserver = NULL;
}

int HostHID::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTHID_REGISTER_HOSTOBSERVER:
		{
			hostObserver = *((HostHIDObserver *) data);
			return true;
		}
		case HOSTHID_POST_LED_EVENT:
		{
			hostObserver(HOSTHID_LED_EVENT, data);
			return true;
		}
	}
	
	return false;
}
