
/**
 * libemulator
 * Component Board
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef COMPONENTBOARD_H
#define COMPONENTBOARD_H

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
