
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
	VIDEOMONITOR_GET_FRAME,
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
	
	float videoHorizStart;
	float videoHorizEnde;
	float videoVertStart;
	float videoVertEnd;
	
	float compositeSubcarrier;
	bool compositeNonInterleaving;
} VideoFrame;

typedef struct
{
	int screenWidth;
	int screenHeight;
	
	float screenBrigthness;
	float screenContrast;
	float screenSaturation;
	bool screenColorize;
	
	float screenHorizStart;
	float screenHorizEnd;
	float screenVertStart;
	float screenVertEnd;
	float screenPersistance;
	float screenFlicker;
	
	float compositeLumaBandwidth;
	float compositeChromaBandwidth;
	float compositeDecoderMatrix[9];
	float compositeScanlineAlpha;
	
	float componentBandwidth;
} MonitorSetting;
