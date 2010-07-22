
/**
 * libemulation
 * OEDML
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Encapsulates a DML document.
 */

#include "OEDML.h"

OEDML::OEDML()
{
	init();
}

OEDML::OEDML(string path)
{
	init();
	open(path);
}

OEDML::~OEDML()
{
	close();
	
	if (doc)
		xmlFreeDoc(doc);
}

void OEDML::init()
{
	openState = false;
	package = NULL;
	doc = NULL;
}

bool OEDML::open(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		if (!(openState = readFile(path, &data)))
			OELog("could not open \"" + path + "\"");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			if (!(openState = package->readFile(OE_PACKAGE_DML_FILENAME,
												&data)))
				OELog("could not read " + OE_PACKAGE_DML_FILENAME +
					  " in \"" + path + "\"");
		}
		else
			OELog("could not open package \"" + path + "\"");
	}
	else
		OELog("could not identify type of \"" + path + "\"");
	
	if (openState)
	{
		doc = xmlReadMemory(data[0], data.size(),
							OE_PACKAGE_DML_FILENAME, NULL, 0)
		if (!doc)
		{
			openState = false;
			OELog("could not parse DML of \"" + path + "\"");
		}
	}
	
	if (openState)
	{
		if (!openState = validate())
			OELog("unknown DML version");
	}
	
	if (!openState)
		close();
	
	return openState;
}

bool OEDML::isOpen()
{
	return openState;
}

bool OEDML::save(string path)
{
	close();
	
	OEData data;
	string pathExtension = getPathExtension(path);
	if (pathExtension == OE_STANDALONE_EXTENSION)
	{
		update();
		
		if (dump(data))
		{
			if (!(openState = writeFile(path, &data)))
				OELog("could not open \"" + path + "\"");
		}
		else
			OELog("could not dump DML for \"" + path + "\"");
	}
	else if (pathExtension == OE_PACKAGE_EXTENSION)
	{
		package = new OEPackage(path);
		if (package && package->isOpen())
		{
			update();
			
			if (dump(data))
			{
				if (!(openState = package->writeFile(OE_PACKAGE_DML_FILENAME,
													 data)))
					OELog("could not write " + OE_PACKAGE_DML_FILENAME +
						  " in \"" + path + "\"");
			}
			else
				OELog("could not dump DML for \"" + path + "\"");
		}
		else
			OELog("could not open package \"" + path + "\"");
	}
	else
		OELog("could not identify type of \"" + path + "\"");
	
	if (!openState)
		close();
	
	return openState;
}

void OEDML::close()
{
	openState = false;
	
	if (package)
		delete package;
	package = NULL;
}

//
// DML Operations
//
bool OEDML::validate()
{
	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	return getXMLProperty(rootNode, "version") == "1.0");
}

bool OEDML::dump(OEData *data)
{
	xmlChar *p;
	int size;
	xmlDocDumpMemory(doc, &p, &size);
	
	if (!p)
		return false;
	
	data->resize(size);
	memcpy(data->getData(), p, size);
	
	return true;
}

void OEDML::update()
{
}

//
// Helpers
//
string OEDML::getString(int value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEDML::getPathExtension(string path)
{
	int extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	return path.substr(extensionIndex + 1);
}

bool OEDML::readFile(string path, OEData *data)
{
	bool error = true;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = file.tellg();
		file.seekg(0, ios::beg);
		
		data->resize(size);
		file.read(data.getData(), size);
		error = !file.good();
		file.close();
	}
	
	return !error;
}

bool OEDML::writeFile(string path, OEData *data)
{
	bool error = true;
	
	ofstream file(path.c_str());
	
	if (file.is_open())
	{
		file.write(data.getData(), data.getSize());
		error = !file.good();
		file.close();
	}
	
	return !error;
}

string OEInfo::getXMLProperty(xmlNodePtr node, string name)
{
	char *value = (char *) xmlGetProp(node, BAD_CAST name.c_str());
	string valueString = value ? value : "";
	xmlFree(value);
	
	return valueString;
}

void OEInfo::setXMLProperty(xmlNodePtr node, string name, string value)
{
	xmlSetProp(node, BAD_CAST name.c_str(), BAD_CAST value.c_str());
}
