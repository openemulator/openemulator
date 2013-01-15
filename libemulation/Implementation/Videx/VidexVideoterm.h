
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include "MC6845.h"

class VidexVideoterm : public MC6845
{
public:
	VidexVideoterm();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    void update();
    void dispose();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *ram;
    OEComponent *bankSwitcher;
    OEComponent *video;
    OEComponent *gamePort;
    
    OEInt ramBank;
    string characterSet1;
    string characterSet2;
    
    bool an0;
    bool colorKiller;
    
    bool videoInhibited;
    bool monitorConnected;
    bool monitorCaptured;
    OESInt cellWidth;
    
    map<string, OEData> font;
    OEData currentFont;
    
    OEChar *drawFont;
    
    OEData *vram;
    
    OEImage image;
    OEChar *imagep;
    OEInt imageWidth;
    
    OEChar *drawMemory;
    
    OEInt videoOutput;
    
    bool loadFont(string name, OEData *data);
    
    void setRAMBank(OEInt value);
    void updateRAMBank();
    
    void setCellWidth(OEInt value);
    
    void updateVideoEnabled();
    
    void updateTiming();
    
    void drawLine(OESInt y, OESInt x0, OESInt x1);
    
    void postImage();
    void copy(wstring *s);
};
