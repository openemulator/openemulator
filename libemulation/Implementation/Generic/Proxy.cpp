
/**
 * libemulation
 * Proxy
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a message proxy
 */

#include "Proxy.h"

Proxy::Proxy()
{
}

bool Proxy::setRef(string name, OEComponent *ref)
{
    if (name == "component")
        component = ref;
    else
        return false;
    
    return true;
}

bool Proxy::init()
{
    if (!component)
    {
        logMessage("component not connected");
        
        return false;
    }
    
    return true;
}

bool Proxy::postMessage(OEComponent *sender, int message, void *data)
{
    return component->postMessage(this, message, data);
}

bool Proxy::addObserver(OEComponent *observer, int notification)
{
    if (!component->addObserver(observer, notification))
        return false;
    
    return OEComponent::addObserver(observer, notification);
}

bool Proxy::removeObserver(OEComponent *observer, int notification)
{
    if (!component->removeObserver(observer, notification))
        return false;
    
    return OEComponent::addObserver(observer, notification);
}

void Proxy::notify(OEComponent *sender, int notification, void *data)
{
    postNotification(this, notification, data);
}
