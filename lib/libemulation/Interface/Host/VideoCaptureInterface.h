
/**
 * libemulation
 * Video capture interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the video capture interface
 */

// Notes:
// * getFrame returns an OEImage.

#ifndef _VIDEOCAPTUREINTERFACE_H
#define _VIDEOCAPTUREINTERFACE_H

typedef enum
{
    VIDEOCAPTURE_GET_FRAME,
} VideoMessage;

#endif
