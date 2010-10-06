
/**
 * libemulation
 * Video Monitor Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the video monitor interface
 */

// Events
enum
{
	VIDEOMONITOR_RENDER_FRAME,
	VIDEOMONITOR_SET_CAPTURE_ON_MOUSE_OVER,
};

typedef enum
{
	VIDEO_FORMAT_COMPOSITE,
	VIDEO_FORMAT_RGB,
} VideoMonitorFrameFormat;

typedef struct
{
	VideoFrameFormat frameFormat;
	int frameWidth;
	int frameHeight;
	char *frameData;
	
	float videoHorizTotal;
	float videoHorizStart;
	float videoHorizDisplayed;
	float videoVertTotal;
	float videoVertStart;
	float videoVertDisplayed;
	
	float compositeSubcarrier;
	bool compositeScanline;
} VideoFrame;

typedef struct
{
	int screenWidth;
	int screenHeight;
	
	float screenBrigthness;
	float screenContrast;
	float screenSaturation;
	
	float screenPersistance;
	float screen;
	
	float compositeLumaBandwidth;
	float compositeChromaBandwidth;
	float compositeScanlineAlpha;
	float compositeDecoderMatrix[9];
} MonitorSetting;
