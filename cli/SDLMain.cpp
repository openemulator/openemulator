
/**
 * AppleIIGo
 * SDL main
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "version.h"
#include "config.h"

#include "SDLIO.h"
#include "SDLIOMessages.h"

static SDLIO * SDL_IO;

void printUsage()
{
	cout << "usage: appleiigo [-h | -v | -r resourcepath] [profile] "
	"[diskimage ...]" << endl;
}

void printVersion()
{
	cout << "AppleIIGo Version " << APPLEIIGO_VERSION
	" (build " APPLEIIGO_BUILD ")" << endl;
}

void trim(string &str) 
{
    size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");
	
    if ((start == string::npos) || (end == string::npos))  
        str = "";
    else  
        str = str.substr(start, end - start + 1);
}

extern "C" uint32_t isProfilePath(char * path)
{
	// To-Do: Remove trailing /
	string stringPath = path;
	string::size_type lastDot = stringPath.find_last_of(".");
	if (lastDot == string::npos)
		return 0;
	
	return (stringPath.substr(lastDot) == PROFILE_EXTENSION);	
}

bool parseCommandline(int argc, char * argv[], map<string, string> &parameters)
{
	char option;
	while ((option = getopt(argc, argv, "hvr:f")) != -1)
	{
		string argument;
		if (optarg)
		{
			argument = optarg;
			trim(argument);
		}
		
		switch (option)
		{
			case 'h':
				printUsage();
				return false;
			case 'v':
				printVersion();
				return false;
			case 'r':
				parameters["resourcePath"] = argument;
				break;
			case '?':
				printUsage();
				return false;
			default:
				abort();
				break;
		}
	}
	
	if (!parameters["resourcePath"].size())
		parameters["resourcePath"] = RESOURCE_PATH;
	
	if (!parameters["profilePath"].size())
		parameters["profilePath"] = DEFAULT_PROFILE_PATH;
	
	for (uint32_t index = optind, diskImageIndex = 0; index < argc; index++)
	{
		string argument = argv[index];
		trim(argument);
		
		if (isProfilePath(argv[index]))
			parameters["profilePath"] = argument;
		else
		{
			ostringstream indexStream;
			indexStream << diskImageIndex++;
			string parameterName = string("diskImage") + indexStream.str();
			parameters[parameterName] = argument;
		}
	}
	
	return true;
}

extern "C" void sendSdlIoMessage(uint32_t sdlIoMessage)
{
	SDL_IO->sendSdlIoMessage(sdlIoMessage, 0);
}

extern "C" void sendComponentMessage(char * destination, void * messageData)
{
	string destinationString = destination;
	SDL_IO->sendComponentMessage(destinationString, messageData);
}

int main(int argc, char * argv[])
{
	map<string, string> parameters;
	if (!parseCommandline(argc, argv, parameters))
		return 1;
	
/*	// DEBUGGING-START
	for (uint32_t index = 0; index < argc; index++)
		cout << index << ": " << argv[index] << endl;
	
	map<string,string>::iterator iter;
	for(iter = parameters.begin(); iter != parameters.end(); iter++)
		cout << (*iter).first << ": " << iter->second << endl;    
	// DEBUGGING-END*/
	
	SDL_IO = new SDLIO(parameters);
	if (!SDL_IO)
		return 2;
	
	if (!SDL_IO->run())
		return 3;
	
	delete SDL_IO;
	
	return 0;
}
