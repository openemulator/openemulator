
/**
 * libemulation
 * Video interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the video interface
 */

#ifndef _VIDEOINTERFACE_H
#define _VIDEOINTERFACE_H

typedef enum
{
	VIDEO_GET_FRAME,
} VideoMessage;

typedef struct
{
	char *frameData;
	int frameWidth;
	int frameHeight;
} VideoFrame;

#endif
