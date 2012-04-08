
/**
 * libdiskimage
 * Disk image library
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Emulation disk image library
 */

/*
 * Common properties:
 *
 *   readOnly
 *
 * Block Disk Image properties:
 *
 *   imageSize (in bytes)
 *
 * Bit Disk Image properties:
 *
 *   diskSize (can be 8", 5.25", 3.5", 3")
 *   trackNum
 *   headNum
 *   rotationSpeed (in RPM)
 *   tracksPerInch
 *
 * System specific properties:
 *   gcrVolume (Apple II)
 *   gcrFormat (Apple II)
 */

// Ideas:
// * BlockDiskImages are meant for block devies.
//   They are accessed on the fly.
// * BitDiskImages are meant for streaming devices.
//   They are 
// * When writing a dsk file and not every sector is recognized,
//   the original file is automatically closed, and an .fdi file
//   is created instead

#include "BlockDiskImage.h"
#include "BitDiskImage.h"
