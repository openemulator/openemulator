
/**
 * libemulator
 * DML Reference
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEREF_H
#define _OEREF_H

#define OE_DEVICE_SEP "::"
#define OE_COMPONENT_SEP "."

using namespace std;

class OERef
{
public:
	OERef();
	OERef(string stringRef);
	OERef(const OERef &ref);
	OERef &operator=(const OERef &ref);
	bool operator==(const OERef &ref);
	
	bool isValid();
	
	string getDevice() const;
	string getComponent() const;
	string getProperty() const;
	
	string getStringRef();
	OERef buildRef(string relativeStringRef);
	
private:
	string device;
	string component;
	string property;
};

ostream &operator<<(ostream &stream, const OERef &ref);

#endif
