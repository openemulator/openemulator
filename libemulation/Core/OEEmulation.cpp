
/**
 * libemulation
 * OEEmulation
 * (C) 2009-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an OpenEmulator emulation
 */

#include <fstream>
#include <sstream>
#include <set>

#include <libxml/parser.h>

#include "OEEmulation.h"
#include "OEDevice.h"

#include "OEComponentFactory.h"

OEEmulation::OEEmulation() : OEDocument()
{
    constructCanvas = NULL;
    destroyCanvas = NULL;
    didUpdate = NULL;
    
    activityCount = 0;
    
    addComponent("emulation", this);
}

OEEmulation::~OEEmulation()
{
    close();
    
    didUpdate = NULL;
    
    if (doc)
    {
        disposeDocument(doc);
        
        deconfigureDocument(doc);
        
        destroyDocument(doc);
    }
}

void OEEmulation::setResourcePath(string path)
{
    resourcePath = path;
}

void OEEmulation::setConstructCanvas(EmulationConstructCanvas constructCanvas)
{
    this->constructCanvas = constructCanvas;
}

void OEEmulation::setDestroyCanvas(EmulationDestroyCanvas destroyCanvas)
{
    this->destroyCanvas = destroyCanvas;
}

void OEEmulation::setDidUpdate(EmulationDidUpdate didUpdate)
{
    this->didUpdate = didUpdate;
}

void OEEmulation::setUserData(void *userData)
{
    this->userData = userData;
}

bool OEEmulation::addComponent(string id, OEComponent *component)
{
    if (!component)
        return false;
    
    if (componentsMap.count(id))
        return false;
    
    componentsMap[id] = component;
    
    return true;
}

bool OEEmulation::removeComponent(string id)
{
    if (!componentsMap.count(id))
        return false;
    
    componentsMap.erase(id);
    
    return true;
}

OEComponent *OEEmulation::getComponent(string id)
{
    if (!componentsMap.count(id))
        return NULL;
    
    return componentsMap[id];
}

string OEEmulation::getId(OEComponent *component)
{
    for (map<string, OEComponent *>::iterator i = componentsMap.begin();
         i != componentsMap.end();
         i++)
    {
        if (i->second == component)
            return i->first;
    }
    
    return "";
}

bool OEEmulation::isActive()
{
    return (activityCount != 0);
}



bool OEEmulation::constructDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "device")
        {
            string deviceId = getNodeProperty(node, "id");
            
            if (!constructDevice(deviceId))
                return false;
        }
        else if (getNodeName(node) == "component")
        {
            string id = getNodeProperty(node, "id");
            string className = getNodeProperty(node, "class");
            
            if (!constructComponent(id, className))
                return false;
        }
    }
    
    if (configureDocument(doc))
        if (initDocument(doc))
            return true;
    
    return false;
}

bool OEEmulation::constructDevice(string deviceId)
{
    if (!getComponent(deviceId))
    {
        OEComponent *component;
        component = new OEDevice(this);
        
        if (component && addComponent(deviceId, component))
            return true;
        else
            logMessage("could not construct '" + deviceId + "'");
    }
    else
        logMessage("redefinition of '" + deviceId + "'");
    
    return false;
}

bool OEEmulation::constructComponent(string id, string className)
{
    if (!getComponent(id))
    {
        OEComponent *component;
        component = OEComponentFactory::construct(className);
        
        if (component)
            return addComponent(id, component);
        else
            logMessage("could not construct '" + id +
                       "', class '" + className + "' is not defined");
    }
    else
        logMessage("redefinition of '" + id + "'");
    
    return false;
}

bool OEEmulation::configureDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "device")
        {
            string deviceId = getNodeProperty(node, "id");
            string label = getNodeProperty(node, "label");
            string image = getNodeProperty(node, "image");
            
            if (!configureDevice(deviceId, label, image, node->children))
                return false;
        }
        else if (getNodeName(node) == "component")
        {
            string id = getNodeProperty(node, "id");
            
            if (!configureComponent(id, node->children))
                return false;
        }
    }
    
    return true;
}

bool OEEmulation::configureDevice(string deviceId,
                                  string label, string image,
                                  xmlNodePtr children)
{
    OEComponent *device = getComponent(deviceId);
    string locationLabel = getLocationLabel(deviceId);
    
    // Parse settings
    DeviceSettings settings;
    for(xmlNodePtr node = children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "setting")
        {
            string ref = getNodeProperty(node, "ref");
            
            OEComponent *component = getComponent(ref);
            
            if (!component)
            {
                logMessage("'" + ref + "' was not declared");
                
                return false;
            }
            
            DeviceSetting setting;
            
            setting.component = component;
            setting.name = getNodeProperty(node, "name");
            setting.type = getNodeProperty(node, "type");
            setting.options = getNodeProperty(node, "options");
            setting.label = getNodeProperty(node, "label");
            
            settings.push_back(setting);
        }
    }
    
    device->postMessage(this, DEVICE_SET_LABEL, &label);
    device->postMessage(this, DEVICE_SET_IMAGEPATH, &image);
    device->postMessage(this, DEVICE_SET_LOCATIONLABEL, &locationLabel);
    device->postMessage(this, DEVICE_SET_SETTINGS, &settings);
    
    return true;
}

bool OEEmulation::configureComponent(string id, xmlNodePtr children)
{
    OEComponent *component = getComponent(id);
    if (!component)
    {
        logMessage("'" + id + "' was not declared");
        
        return false;
    }
    
    map<string, string> propertiesMap;
    propertiesMap["id"] = id;
    propertiesMap["deviceId"] = OEGetDeviceId(id);
    propertiesMap["resourcePath"] = resourcePath;
    
    for(xmlNodePtr node = children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "property")
        {
            string name = getNodeProperty(node, "name");
            
            if (hasNodeProperty(node, "value"))
            {
                string value = getNodeProperty(node, "value");
                value = parseValueProperties(value, propertiesMap);
                
                if (!component->setValue(name, value))
                    logMessage("could not set value property '" + name + "' for '" + id + "'");
            }
            else if (hasNodeProperty(node, "ref"))
            {
                string refId = getNodeProperty(node, "ref");
                
                OEComponent *ref = getComponent(refId);
                
                if ((refId != "") && !ref)
                    logMessage("'" + refId + "' was not declared");
                
                if (!component->setRef(name, ref))
                    logMessage("could not set ref property '" + name + "' for '" + id + "'");
            }
            else if (hasNodeProperty(node, "data"))
            {
                string dataSrc = getNodeProperty(node, "data");
                
                OEData data;
                
                string parsedSrc = parseValueProperties(dataSrc, propertiesMap);
                
                bool dataRead = false;
                
                if (hasValueProperty(dataSrc, "packagePath"))
                {
                    if (package)
                        dataRead = package->readFile(parsedSrc, &data);
                }
                else
                    dataRead = readFile(parsedSrc, &data);
                
                if (dataRead)
                {
                    if (!component->setData(name, &data))
                        logMessage("could not set data property '" + name + "' for '" + id + "'");
                }
            }
            else
                logMessage("could not recognize type of property '" + name + "' in '" + id + "'");
        }
    }
    
    return true;
}

bool OEEmulation::configureInlets(OEInletMap& inletMap)
{
    set<OEComponent *> components;
    
    for (OEInletMap::iterator i = inletMap.begin();
         i != inletMap.end();
         i++)
    {
        string id = i->first;
        OEIdMap& idMap = i->second;
        
        OEComponent *component = getComponent(id);
        
        if (!component)
        {
            logMessage("'" + id + "' was not declared");
            
            continue;
        }
        
        for (OEIdMap::iterator j = idMap.begin();
             j != idMap.end();
             j++)
        {
            string name = j->first;
            string refId = j->second;
            
            OEComponent *ref = getComponent(refId);
            
            if (!component->setRef(name, ref))
                logMessage("could not set ref property '" + name + "' for '" + id + "'");
            else
                components.insert(component);
        }
    }
    
    for (set<OEComponent *>::iterator i = components.begin();
         i != components.end();
         i++)
        (*i)->update();
    
    return true;
}

bool OEEmulation::initDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "component")
        {
            string id = getNodeProperty(node, "id");
            
            if (!initComponent(id))
                return false;
        }
    }
    
    return true;
}

bool OEEmulation::initComponent(string id)
{
    OEComponent *component = getComponent(id);
    
    if (!component)
    {
        logMessage("'" + id + "' was not declared");
        
        return false;
    }
    
    if (!component->init())
    {
        logMessage("could not init '" + id + "'");
        
        return false;
    }
    
    component->update();
    
    return true;
}

bool OEEmulation::updateDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
        if (getNodeName(node) == "component")
        {
            string id = getNodeProperty(node, "id");
            
            if (!updateComponent(id, node->children))
                return false;
        }
    
    return true;
}

bool OEEmulation::updateComponent(string id, xmlNodePtr children)
{
    OEComponent *component = getComponent(id);
    
    if (!component)
    {
        logMessage("'" + id + "' was not declared");
        
        return false;
    }
    
    map<string, string> propertiesMap;
    propertiesMap["id"] = id;
    propertiesMap["deviceId"] = OEGetDeviceId(id);
    propertiesMap["resourcePath"] = resourcePath;
    
    for(xmlNodePtr node = children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "property")
        {
            string name = getNodeProperty(node, "name");
            
            if (hasNodeProperty(node, "value"))
            {
                string value;
                
                if (component->getValue(name, value))
                    setNodeProperty(node, "value", value);
            }
            else if (hasNodeProperty(node, "data"))
            {
                string dataSrc = getNodeProperty(node, "data");
                
                if (!hasValueProperty(dataSrc, "packagePath") || !package)
                    continue;
                
                OEData *data = NULL;
                
                string parsedSrc = parseValueProperties(dataSrc, propertiesMap);
                
                if (component->getData(name, &data) && data)
                {
                    if (!package->writeFile(parsedSrc, data))
                        logMessage("could not store data property '" + dataSrc + "'");
                }
            }
        }
    }
    
    return true;
}

void OEEmulation::disposeDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if ((getNodeName(node) == "component") ||
            (getNodeName(node) == "device"))
        {
            string id = getNodeProperty(node, "id");
            
            disposeComponent(id);
        }
    }
}

void OEEmulation::disposeDevice(string deviceId)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if ((getNodeName(node) == "component") ||
            (getNodeName(node) == "device"))
        {
            string componentId = getNodeProperty(node, "id");
            
            if (OEGetDeviceId(componentId) == deviceId)
                disposeComponent(componentId);
        }
    }
}

void OEEmulation::disposeComponent(string id)
{
    OEComponent *component = getComponent(id);
    
    if (!component)
        return;
    
    component->dispose();
}

void OEEmulation::deconfigureDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "component")
        {
            string id = getNodeProperty(node, "id");
            
            deconfigureComponent(id, node->children);
        }
    }
}

void OEEmulation::deconfigureDevice(string deviceId)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "component")
        {
            string componentId = getNodeProperty(node, "id");
            
            for(xmlNodePtr propertyNode = node->children;
                propertyNode;
                propertyNode = propertyNode->next)
            {
                string ref = getNodeProperty(propertyNode, "ref");
                
                if ((OEGetDeviceId(componentId) == deviceId) ||
                    (OEGetDeviceId(ref) == deviceId))
                {
                    OEComponent *component = getComponent(componentId);
                    if (component)
                    {
                        string name = getNodeProperty(propertyNode, "name");
                        
                        component->setRef(name, NULL);
                    }
                }
            }
        }
    }
}

void OEEmulation::deconfigureComponent(string id, xmlNodePtr children)
{
    OEComponent *component = getComponent(id);
    
    if (!component)
        return;
    
    for(xmlNodePtr node = children;
        node;
        node = node->next)
    {
        if (getNodeName(node) == "property")
        {
            string name = getNodeProperty(node, "name");
            
            if (hasNodeProperty(node, "ref"))
                component->setRef(name, NULL);
        }
    }
}

void OEEmulation::destroyDocument(xmlDocPtr doc)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if ((getNodeName(node) == "component") ||
            (getNodeName(node) == "device"))
        {
            string componentId = getNodeProperty(node, "id");
            
            destroyComponent(componentId, node->children);
        }
    }
}

void OEEmulation::destroyDevice(string deviceId)
{
    xmlNodePtr rootNode = xmlDocGetRootElement(doc);
    
    for(xmlNodePtr node = rootNode->children;
        node;
        node = node->next)
    {
        if ((getNodeName(node) == "component") ||
            (getNodeName(node) == "device"))
        {
            string componentId = getNodeProperty(node, "id");
            
            if (OEGetDeviceId(componentId) == deviceId)
                destroyComponent(componentId, node->children);
        }
    }
}

void OEEmulation::destroyComponent(string id, xmlNodePtr children)
{
    OEComponent *component = getComponent(id);
    
    if (!component)
        return;
    
    delete component;
    
    removeComponent(id);
}

bool OEEmulation::hasValueProperty(string value, string propertyName)
{
    return (value.find("${" + propertyName + "}") != string::npos);
}

string OEEmulation::parseValueProperties(string value, map<string, string>& propertiesMap)
{
    size_t startIndex;
    
    while ((startIndex = value.find("${")) != string::npos)
    {
        size_t endIndex = value.find("}", startIndex);
        if (endIndex == string::npos)
        {
            value = value.substr(0, startIndex);
            break;
        }
        
        string propertyName = value.substr(startIndex + 2,
                                           endIndex - startIndex - 2);
        string replacement;
        
        for (map<string, string>::iterator i = propertiesMap.begin();
             i != propertiesMap.end();
             i++)
        {
            if (i->first == propertyName)
                replacement = i->second;
        }
        
        value = value.replace(startIndex,
                              endIndex - startIndex + 1,
                              replacement);
    }
    
    return value;
}
