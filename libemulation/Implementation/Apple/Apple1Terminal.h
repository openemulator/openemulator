
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "OEComponent.h"

#include "OEImage.h"

class Apple1Terminal : public OEComponent
{
public:
	Apple1Terminal();
	
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
	void write(int address, int value);
	
private:
	OEComponent *vram;
	OEComponent *controlBus;
	OEComponent *monitor;
	
    bool speedLimit;
    bool cursorActive;
    int cursorX, cursorY;
    
	OEData font;
    char *vramp;
    OEImage image;
    
    void scheduleTimer();
    void loadFont(OEData *data);
    void updateCanvas();
    void sendKey(int unicode);
    void copy(string *s);
    void paste(string *s);
};
