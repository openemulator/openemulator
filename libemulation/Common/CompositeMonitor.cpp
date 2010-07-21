
/**
 * libemulation
 * Composite Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a composite monitor.
 */

#include "CompositeMonitor.h"


/*
 int *framebuffer = new int[framebufferWidth * framebufferHeight];
 int *p = framebuffer;
 
 for (int y = 0; y < framebufferHeight; y++)
 {
 float kk = 1;
 
 for (int x = 0; x < framebufferWidth; x++)
 {
 float l;
 //			float l = (((x >> 0) & 0x1) ^ ((y >> 1) & 0x1)) * 1.0;
 kk = 0.99 * kk;
 l = kk;
 
 if (y % 2)
 l *= 0.5;
 
 int r = l * 0x33;
 int g = l * 0xcc;
 int b = l * 0x44;
 *p++ = (b << 16) | (g << 8) | r;
 }
 }
 delete framebuffer;
 
 
 */