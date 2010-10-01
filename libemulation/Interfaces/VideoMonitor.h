
/**
 * libemulation
 * Control bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the serial port interface
 */

// Events
enum
{
	VIDEOMONITOR_RENDER_FRAME,
	VIDEOMONITOR_SET_CAPTURE_ON_MOUSE_OVER,
};

typedef enum
{
	VIDEOMONITOR_FORMAT_1BIT,
	VIDEOMONITOR_FORMAT_4BIT,
	VIDEOMONITOR_FORMAT_8BIT,
	VIDEOMONITOR_FORMAT_16BIT,
	VIDEOMONITOR_FORMAT_32BIT,
} VideoMonitorFrameFormat;

typedef struct
{
	char *framebuffer;
	int framebufferWidth;
	int framebufferHeight;
	VideoMonitorFrameFormat framebufferFormat;
	
	bool color;
	int *palette;
} VideoMonitorFrame;
