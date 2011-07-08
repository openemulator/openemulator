
/**
 * OpenEmulator
 * Mac OS X Emulation Item
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation item.
 */

#import "EmulationItem.h"
#import "StringConversion.h"

#import "OEEmulation.h"

#import "DeviceInterface.h"
#import "StorageInterface.h"

 @implementation EmulationItem

- (EmulationItem *)getGroup:(NSString *)group
{
	for (EmulationItem *item in children)
	{
		if ([[item uid] compare:group] == NSOrderedSame)
			return item;
	}
	
    EmulationItem *item = [[EmulationItem alloc] initGroup:group];
    [children addObject:item];
    [item release];
    
    return item;
}

- (id)initRootWithDocument:(Document *)theDocument
{
    self = [super init];
    
	if (self)
	{
		type = EMULATIONITEM_ROOT;
		uid = [@"" copy];
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
        if (![NSThread isMainThread])
            [document lockEmulation];
        
		// Get info
		OEEmulation *emulation = (OEEmulation *)[theDocument emulation];
		OEIds deviceIds = emulation->getDeviceIds();
        
		OEPortsInfo portsInfo;
		portsInfo = emulation->getPortsInfo();
        
		// Create items connected on ports
        EmulationItem *systemGroupItem = [self getGroup:@"system"];
        
        for (OEPortsInfo::iterator i = portsInfo.begin();
             i != portsInfo.end();
             i++)
        {
            OEPortInfo port = *i;
            string deviceId = emulation->getDeviceId(port.ref);
			OEComponent *theComponent = emulation->getComponent(deviceId);
            
            EmulationItem *item;
            OEIds::iterator foundDeviceId;
            foundDeviceId = find(deviceIds.begin(), deviceIds.end(), deviceId);
            if (theComponent && (foundDeviceId != deviceIds.end()))
            {
                item = [[EmulationItem alloc] initDevice:getNSString(deviceId)
                                               component:theComponent
                                                portType:getNSString(port.type)
                                                  portId:getNSString(port.id)
                                                document:theDocument];
                
                deviceIds.erase(foundDeviceId);
            }
            else
                item = [[EmulationItem alloc] initPort:getNSString(port.id)
                                                 label:getNSString(port.label)
                                             imagePath:getNSString(port.image)
                                              portType:getNSString(port.type)
                                              document:theDocument];
            
            string group = port.group;
            if (group == "")
                group = "Unknown";
            EmulationItem *groupItem = [self getGroup:getNSString(group)];
			NSMutableArray *groupChildren = [groupItem children];
            [groupChildren addObject:item];
            
            [item release];
        }
        
        // Create items not connected on ports
        for (OEIds::iterator i = deviceIds.begin();
             i != deviceIds.end();
             i++)
        {
            string deviceId = *i;
			OEComponent *theComponent = emulation->getComponent(deviceId);
            
            EmulationItem *item;
            item = [[EmulationItem alloc] initDevice:getNSString(deviceId)
                                           component:theComponent
                                            portType:@""
                                              portId:@""
                                            document:theDocument];
            
			NSMutableArray *systemGroupChildren = [systemGroupItem children];
            [systemGroupChildren addObject:item];
            
            [item release];
        }
        
        if (![NSThread isMainThread])
            [document unlockEmulation];
	}
	
	return self;
}

- (id)initGroup:(NSString *)theUID
{
	if ((self = [super init]))
	{
		type = EMULATIONITEM_GROUP;
		uid = [theUID copy];
		children = [[NSMutableArray alloc] init];
		
		label = [[NSLocalizedString(theUID, @"Emulation Item Group Label.")
				  uppercaseString] retain];
	}
	
	return self;
}

- (id)initDevice:(NSString *)theUID
       component:(void *)theComponent
        portType:(NSString *)thePortType
          portId:(NSString *)thePortId
		document:(Document *)theDocument
{
	if ((self = [super init]))
	{
		type = EMULATIONITEM_DEVICE;
		uid = [theUID copy];
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		device = theComponent;
        
		// Read device values
		string value;
        
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_LABEL, &value);
		label = [getNSString(value) retain];
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_IMAGEPATH, &value);
		NSString *imagePath = [[resourcePath stringByAppendingPathComponent:
								getNSString(value)] retain];
		image = [[NSImage alloc] initByReferencingFile:imagePath];
		
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_LOCATIONLABEL, &value);
		locationLabel = [getNSString(value) retain];
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_STATELABEL, &value);
		stateLabel = [getNSString(value) retain];
		
		// Read settings
		settingsRef = [[NSMutableArray alloc] init];
		settingsName = [[NSMutableArray alloc] init];
		settingsLabel = [[NSMutableArray alloc] init];
		settingsType = [[NSMutableArray alloc] init];
		settingsOptions = [[NSMutableArray alloc] init];
		DeviceSettings settings;
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_SETTINGS, &settings);
		for (int i = 0; i < settings.size(); i++)
		{
			DeviceSetting setting = settings[i];
			[settingsRef addObject:getNSString(setting.ref)];
			[settingsName addObject:getNSString(setting.name)];
			[settingsLabel addObject:getNSString(setting.label)];
			[settingsType addObject:getNSString(setting.type)];
			[settingsOptions addObject:[getNSString(setting.options)
										componentsSeparatedByString:@","]];
		}
		
		// Read canvases
		canvases = [[NSMutableArray alloc] init];
		OEComponents theCanvases;
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_CANVASES, &theCanvases);
		for (int i = 0; i < theCanvases.size(); i++)
			[canvases addObject:[NSValue valueWithPointer:theCanvases.at(i)]];
		
		// Read storages
		storages = [[NSMutableArray alloc] init];
		OEComponents theStorages;
		((OEComponent *)device)->postMessage(NULL, DEVICE_GET_STORAGES, &theStorages);
		for (int i = 0; i < theStorages.size(); i++)
		{
			OEComponent *theComponent = theStorages.at(i);
			[storages addObject:[NSValue valueWithPointer:theComponent]];
			
			((OEComponent *)theComponent)->postMessage(NULL, STORAGE_GET_MOUNTPATH, &value);
			if (value.size())
			{
				NSString *storageUID;
				storageUID = [NSString stringWithFormat:@"%@.storage", uid];
				
				EmulationItem *storageItem;
				storageItem = [[EmulationItem alloc] initMount:storageUID
                                                     component:theComponent
                                                 locationLabel:locationLabel
                                                      document:theDocument];
				[children addObject:storageItem];
				[storageItem release];
			}
		}
        
		portType = [thePortType copy];
        portId = [thePortId copy];
	}
	
	return self;
}

- (id)initMount:(NSString *)theUID
      component:(void *)theComponent
  locationLabel:(NSString *)theLocationLabel
       document:(Document *)theDocument
{
	if ((self = [super init]))
	{
		type = EMULATIONITEM_MOUNT;
		uid = [theUID copy];
		children = [[NSMutableArray alloc] init];
		document = theDocument;
		
		string value;
        
		((OEComponent *)theComponent)->postMessage(NULL, STORAGE_GET_MOUNTPATH, &value);
		label = [[getNSString(value) lastPathComponent] retain];
		image = [[NSImage imageNamed:@"DiskImage"] retain];
		
		locationLabel = [theLocationLabel copy];
		value = "";
		((OEComponent *)theComponent)->postMessage(NULL, STORAGE_GET_FORMATLABEL, &value);
		stateLabel = [getNSString(value) retain];
		
		storages = [[NSMutableArray alloc] init];
		[storages addObject:[NSValue valueWithPointer:theComponent]];
	}
	
	return self;
}

- (id)initPort:(NSString *)theUID
         label:(NSString *)theLabel
     imagePath:(NSString *)theImagePath
      portType:(NSString *)thePortType
      document:(Document *)theDocument;
{
	if ((self = [super init]))
	{
		type = EMULATIONITEM_AVAILABLEPORT;
		uid = [theUID copy];
		children = [[NSMutableArray alloc] init];
		document = theDocument;
        
        if ([theLabel length])
            theLabel = [@" " stringByAppendingString:theLabel];
		
		OEEmulation *emulation = (OEEmulation *)[theDocument emulation];
		string deviceId = emulation->getDeviceId(getCPPString(uid));
		OEComponent *theDevice = emulation->getComponent(deviceId);
		if (theDevice)
		{
			string deviceLabel;
			theDevice->postMessage(NULL, DEVICE_GET_LABEL, &deviceLabel);
			string theLocationLabel;
			theDevice->postMessage(NULL, DEVICE_GET_LOCATIONLABEL, &theLocationLabel);
			if (theLocationLabel == "")
				locationLabel = [[getNSString(deviceLabel) stringByAppendingFormat:@"%@",
								  theLabel] retain];
			else
				locationLabel = [[getNSString(theLocationLabel) stringByAppendingFormat:@"%@",
								  theLabel] retain];
		}
		NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
		NSString *imagePath = [resourcePath stringByAppendingPathComponent:theImagePath];
		image = [[NSImage alloc] initByReferencingFile:imagePath];
		
		label = [thePortType copy];
		stateLabel = @"";
		
		portType = [thePortType copy];
        portId = [theUID copy];
	}
	
	return self;
}

- (void)dealloc
{
	[uid release];
	[children release];
	
	[label release];
	[image release];
	
	[locationLabel release];
	[stateLabel release];
	
	[settingsRef release];
	[settingsName release];
	[settingsLabel release];
	[settingsType release];
	[settingsOptions release];
	
	[canvases release];
	[storages release];
	
	[portType release];
    [portId release];
	
	[super dealloc];
}



- (BOOL)isGroup
{
	return (type == EMULATIONITEM_GROUP);
}

- (NSString *)uid
{
	return [[uid copy] autorelease];
}

- (NSMutableArray *)children
{
	return children;
}



- (NSString *)label
{
	return label;
}

- (NSImage *)image
{
	return image;
}



- (NSString *)locationLabel
{
	return locationLabel;
}

- (NSString *)stateLabel
{
	return stateLabel;
}



- (void *)device
{
	return device;
}



- (NSInteger)numberOfSettings
{
	return [settingsRef count];
}

- (NSString *)labelForSettingAtIndex:(NSInteger)index
{
	return [settingsLabel objectAtIndex:index];
}

- (NSString *)typeForSettingAtIndex:(NSInteger)index
{
	return [settingsType objectAtIndex:index];
}

- (NSArray *)optionsForSettingAtIndex:(NSInteger)index
{
	return [settingsOptions objectAtIndex:index];
}

- (void)setValue:(NSString *)value forSettingAtIndex:(NSInteger)index;
{
	NSString *settingRef = [settingsRef objectAtIndex:index];
	NSString *settingName = [settingsName objectAtIndex:index];
	NSString *settingType = [settingsType objectAtIndex:index];
	if ([settingType compare:@"select"] == NSOrderedSame)
	{
		NSArray *settingOptions = [settingsOptions objectAtIndex:index];
		value = [settingOptions objectAtIndex:[value integerValue]];
	}
	
	[document lockEmulation];
	OEEmulation *emulation = (OEEmulation *)[document emulation];
	OEComponent *component = emulation->getComponent(getCPPString(settingRef));
	if (component)
	{
		if (component->setValue(getCPPString(settingName), getCPPString(value)))
			component->update();
	}
	[document unlockEmulation];
}

- (NSString *)valueForSettingAtIndex:(NSInteger)index
{
	NSString *settingRef = [settingsRef objectAtIndex:index];
	NSString *settingName = [settingsName objectAtIndex:index];
	NSString *value = @"";
	
	[document lockEmulation];
    
	OEEmulation *emulation = (OEEmulation *)[document emulation];
	OEComponent *component = emulation->getComponent(getCPPString(settingRef));
	if (component)
	{
		string theValue;
		component->getValue(getCPPString(settingName), theValue);
		value = getNSString(theValue);
	}
    
	[document unlockEmulation];
	
	NSString *settingType = [settingsType objectAtIndex:index];
	if ([settingType compare:@"select"] == NSOrderedSame)
	{
		NSArray *options = [settingsOptions objectAtIndex:index];
		value = [NSString stringWithFormat:@"%d", [options indexOfObject:value]];
	}
	
	return value;
}



- (BOOL)isRemovable
{
	return (type == EMULATIONITEM_DEVICE) && ([locationLabel length] != 0);
}

- (void)remove
{
	OEEmulation *emulation = (OEEmulation *)[document emulation];
    
	[document lockEmulation];
    
    emulation->removeDevice(getCPPString(uid));
    
	[document unlockEmulation];
}



- (BOOL)hasCanvases
{
	if (canvases)
		return [canvases count];
	
	return NO;
}

- (void)showCanvases
{
	if (canvases)
		for (int i = 0; i < [canvases count]; i++)
			[document showCanvas:[canvases objectAtIndex:i]];
}



- (BOOL)hasStorages
{
	if ((type == EMULATIONITEM_DEVICE) && [storages count])
		return YES;
	
	return NO;
}

- (BOOL)mount:(NSString *)path
{
	if (!storages)
		return NO;
	
	[document lockEmulation];
	
	string value = getCPPString(path);
	BOOL success = NO;
	for (int i = 0; i < [storages count]; i++)
	{
		OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
												 pointerValue];
		
		if (component->postMessage(NULL, STORAGE_IS_AVAILABLE, NULL))
		{
			success = component->postMessage(NULL, STORAGE_MOUNT, &value);
			
			if (success)
				break;
		}
	}
	
	if (!success)
	{
		for (int i = 0; i < [storages count]; i++)
		{
			OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
													 pointerValue];
			
			success = component->postMessage(NULL, STORAGE_MOUNT, &value);
			
			if (success)
				break;
		}
	}
	
	[document unlockEmulation];
	
	return success;
}

- (BOOL)canMount:(NSString *)path
{
	if (!storages)
		return NO;
	
	[document lockEmulation];
	
	string value = getCPPString(path);
	BOOL success = NO;
	for (int i = 0; i < [storages count]; i++)
	{
		OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
												 pointerValue];
				
		success = component->postMessage(NULL, STORAGE_CAN_MOUNT, &value);
		
		if (success)
			break;
	}
	
	[document unlockEmulation];
	
	return success;
}

- (BOOL)isMount
{
	return (type == EMULATIONITEM_MOUNT);
}

- (void)revealInFinder
{
	if (!storages)
		return;
	
	string value;
	for (int i = 0; i < [storages count]; i++)
	{
		OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
												 pointerValue];
		
		[document lockEmulation];
		
		component->postMessage(NULL, STORAGE_GET_MOUNTPATH, &value);
		
		[document unlockEmulation];
		
		[[NSWorkspace sharedWorkspace] selectFile:getNSString(value)
						 inFileViewerRootedAtPath:@""];
	}
}

- (BOOL)isLocked
{
	if (!storages)
		return NO;
	
	[document lockEmulation];
	
	BOOL success = NO;
	for (int i = 0; i < [storages count]; i++)
	{
		OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
												 pointerValue];
		
		success |= component->postMessage(NULL, STORAGE_IS_LOCKED, NULL);
	}
	
	[document unlockEmulation];
	
	return success;
}

- (void)unmount
{
	if (!storages)
		return;
	
	[document lockEmulation];
	
	for (int i = 0; i < [storages count]; i++)
	{
		OEComponent *component = (OEComponent *)[[storages objectAtIndex:i]
												 pointerValue];
		
		component->postMessage(NULL, STORAGE_UNMOUNT, NULL);
	}
	
	[document unlockEmulation];
	
	return;
}



- (BOOL)isPort
{
	return (type == EMULATIONITEM_AVAILABLEPORT);
}

- (BOOL)addOEDocument:(NSString *)thePath
{
	OEDocument oeDocument;
	oeDocument.open(getCPPString(thePath));
	if (!oeDocument.isOpen())
		return NO;
	
	OEConnectorsInfo connectorsInfo = oeDocument.getFreeConnectorsInfo();
	
	oeDocument.close();
	
	if (connectorsInfo.size() == 1)
	{
        OEConnectorsInfo::iterator i = connectorsInfo.begin();
		
		map<string, string> idMap;
		idMap[getCPPString(portId)] = i->id;
		
		OEEmulation *emulation = (OEEmulation *)[document emulation];
		
		[document lockEmulation];
        
        if (type == EMULATIONITEM_DEVICE)
            emulation->removeDevice(getCPPString(uid));
        
		bool result = emulation->addDocument(getCPPString(thePath), idMap);
        
		[document unlockEmulation];
        
		return result;
	}
	
	return NO;
}

- (NSString *)portType
{
	return portType;
}

@end
