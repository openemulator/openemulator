
/**
 * libemulator
 * Defines
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEDEFINES_H
#define _OEDEFINES_H

#include <string>
#include <vector>

#include "OEComponent.h"

#define OE_SUBSTITUTION_DEVICE_NAME "${DEVICE_NAME}"
#define OE_PATH_SEPARATOR "/"

#define OE_DEVICE_SEP "::"
#define OE_COMPONENT_SEP "."

#define OE_DML_FILENAME "info.xml"

enum {
	OEIoctlSetConnection,
	OEIoctlSetProperty,
	OEIoctlGetProperty,
	OEIoctlSetData,
	OEIoctlGetData,
	OEIoctlSetResource,
	OEIoctlSetEvent,
};

typedef struct
{
	string key;
	OEComponent *component;
} OEIoctlConnection;

typedef struct
{
	string key;
	string value;
} OEIoctlProperty;

typedef struct
{
	string key;
	vector<char> data;
} OEIoctlData;

#endif
