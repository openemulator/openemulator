
/**
 * libemulation
 * Canvas interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the canvas interface
 */

#define HOST_CANVAS_L_NUMLOCK		(1 << 0)
#define HOST_CANVAS_L_CAPSLOCK		(1 << 1)
#define HOST_CANVAS_L_SCROLLLOCK	(1 << 2)
#define HOST_CANVAS_L_COMPOSE		(1 << 3)
#define HOST_CANVAS_L_KANA			(1 << 4)
#define HOST_CANVAS_L_POWER			(1 << 5)
#define HOST_CANVAS_L_SHIFT			(1 << 6)

#define HOST_CANVAS_B_POWER			(1 << 0)
#define HOST_CANVAS_B_PAUSE			(1 << 1)

class OECanvas
{
	virtual void setKeyboardFlags(int flags) = 0;
	virtual void setBadgeFlags(int flags) = 0;
	HostCanvasFrame *getFrame(HostCanvasFormat frameFormat, int frameWidth, int frameHeight) = 0;
	void postFrame(HostCanvasFrame *frame) = 0;
};
