
/**
 * libemulation
 * OEEmulation
 * (C) 2009-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an emulation
 */

#ifndef _OEEMULATION_H
#define _OEEMULATION_H

#include <vector>

#include "OEDocument.h"
#include "OEComponent.h"
#include "OEDevice.h"

using namespace std;

typedef void (*EmulationDidUpdate)(void *userData);
typedef OEComponent *(*EmulationConstructCanvas)(void *userData, OEComponent *device);
typedef void (*EmulationDestroyCanvas)(void *userData, OEComponent *canvas);

typedef map<string, OEComponent *> OEComponentsMap;

class OEEmulation : public OEComponent, public OEDocument
{
public:
    OEEmulation();
    ~OEEmulation();
    
    void setResourcePath(string path);
    void setDidUpdate(EmulationDidUpdate didUpdate);
    void setConstructCanvas(EmulationConstructCanvas constructCanvas);
    void setDestroyCanvas(EmulationDestroyCanvas destroyCanvas);
    void setUserData(void *userData);
    
    bool addComponent(string id, OEComponent *component);
    bool removeComponent(string id);
    OEComponent *getComponent(string id);
    string getId(OEComponent *component);
    
    bool isActive();
    
private:
    string resourcePath;
    OEComponentsMap componentsMap;
    
    EmulationDidUpdate didUpdate;
    EmulationConstructCanvas constructCanvas;
    EmulationDestroyCanvas destroyCanvas;
    void *userData;
    
    OEInt activityCount;
    
    bool constructDocument(xmlDocPtr doc);
    bool constructDevice(string deviceId);
    bool constructComponent(string id, string className);
    bool configureDocument(xmlDocPtr doc);
    bool configureDevice(string deviceId,
                         string label, string image,
                         xmlNodePtr children);
    bool configureInlets(OEInletMap& inletMap);
    bool configureComponent(string id, xmlNodePtr children);
    bool initDocument(xmlDocPtr doc);
    bool initComponent(string id);
    bool reconfigureDocument(xmlDocPtr doc);
    bool reconfigureComponent(string id, xmlNodePtr children);
    void disposeDocument(xmlDocPtr doc);
    void disposeDevice(string deviceId);
    void disposeComponent(string id);
    void deconfigureDocument(xmlDocPtr doc);
    void deconfigureDevice(string deviceId);
    void deconfigureComponent(string id, xmlNodePtr children);
    void destroyDocument(xmlDocPtr doc);
    void destroyDevice(string deviceId);
    void destroyComponent(string id, xmlNodePtr children);
    
    bool hasValueProperty(string value, string propertyName);
    string parseValueProperties(string value, map<string, string>& propertiesMap);
    
    friend class OEDevice;
};

#endif
