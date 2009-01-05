
/**
 * libappleiigo
 * Component Board
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef BREADBOARD_H
#define BREADBOARD_H

#include <string>

using namespace std;

class ComponentBoard {
public:
	ComponentBoard(string &settingsFile);
	~ComponentBoard();
	
	void sendMessage(string &name, void * messageData);
	
private:
	class List * components;
};

#endif
