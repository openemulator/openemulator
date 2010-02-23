
/**
 * OpenEmulator
 * CLI Main
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Program entry point.
 */

#include "stdio.h"
#include "getopt.h"

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "oepa.h"
#include "oegl.h"

typedef struct
{
	int usbUsageId;
	SDLKey sym;
} SDLKeyMapEntry;

SDLKeyMapEntry sdlKeyMap[] = 
{
{0x04, SDLK_a},
{0x05, SDLK_b},
{0x06, SDLK_c},
{0x07, SDLK_d},
{0x08, SDLK_e},
{0x09, SDLK_f},
{0x0a, SDLK_g},
{0x0b, SDLK_h},
{0x0c, SDLK_i},
{0x0d, SDLK_j},
{0x0e, SDLK_k},
{0x0f, SDLK_l},
{0x10, SDLK_m},
{0x11, SDLK_n},
{0x12, SDLK_o},
{0x13, SDLK_p},
{0x14, SDLK_q},
{0x15, SDLK_r},
{0x16, SDLK_s},
{0x17, SDLK_t},
{0x18, SDLK_u},
{0x19, SDLK_v},
{0x1a, SDLK_w},
{0x1b, SDLK_x},
{0x1c, SDLK_y},
{0x1d, SDLK_z},
{0x1e, SDLK_1},
{0x1f, SDLK_2},
{0x20, SDLK_3},
{0x21, SDLK_4},
{0x22, SDLK_5},
{0x23, SDLK_6},
{0x24, SDLK_7},
{0x25, SDLK_8},
{0x26, SDLK_9},
{0x27, SDLK_0},
{0x28, SDLK_RETURN},
{0x29, SDLK_ESCAPE},
{0x2a, SDLK_BACKSPACE},
{0x2b, SDLK_TAB},
{0x2c, SDLK_SPACE},
{0x2d, SDLK_MINUS},
{0x2e, SDLK_EQUALS},
{0x2f, SDLK_LEFTBRACKET},
{0x30, SDLK_RIGHTBRACKET},
{0x31, SDLK_BACKSLASH},

{0x33, SDLK_SEMICOLON},
{0x34, SDLK_QUOTE},
{0x35, SDLK_BACKQUOTE},
{0x36, SDLK_COMMA},
{0x37, SDLK_PERIOD},
{0x38, SDLK_SLASH},
{0x39, SDLK_CAPSLOCK},
{0x3a, SDLK_F1},
{0x3b, SDLK_F2},
{0x3c, SDLK_F3},
{0x3d, SDLK_F4},
{0x3e, SDLK_F5},
{0x3f, SDLK_F6},
{0x40, SDLK_F7},
{0x41, SDLK_F8},
{0x42, SDLK_F9},
{0x43, SDLK_F10},
{0x44, SDLK_F11},
{0x45, SDLK_F12},

{0x47, SDLK_SCROLLOCK},
{0x48, SDLK_PAUSE},
{0x49, SDLK_INSERT},
{0x4a, SDLK_HOME},
{0x4b, SDLK_PAGEUP},
{0x4c, SDLK_DELETE},
{0x4d, SDLK_END},
{0x4e, SDLK_PAGEDOWN},
{0x4f, SDLK_RIGHT},
{0x50, SDLK_LEFT},
{0x51, SDLK_DOWN},
{0x52, SDLK_UP},
{0x53, SDLK_NUMLOCK},
{0x54, SDLK_KP_DIVIDE},
{0x55, SDLK_KP_MULTIPLY},
{0x56, SDLK_KP_MINUS},
{0x57, SDLK_KP_PLUS},
{0x58, SDLK_KP_ENTER},
{0x59, SDLK_KP1},
{0x5a, SDLK_KP2},
{0x5b, SDLK_KP3},
{0x5c, SDLK_KP4},
{0x5d, SDLK_KP5},
{0x5e, SDLK_KP6},
{0x5f, SDLK_KP7},
{0x60, SDLK_KP8},
{0x61, SDLK_KP9},
{0x62, SDLK_KP0},
{0x63, SDLK_KP_PERIOD},

{0x68, SDLK_F13},
{0x69, SDLK_F14},
{0x6a, SDLK_F15},

{0xe0, SDLK_LCTRL},
{0xe1, SDLK_LSHIFT},
{0xe2, SDLK_LALT},
{0xe3, SDLK_LSUPER},
{0xe4, SDLK_RCTRL},
{0xe5, SDLK_RSHIFT},
{0xe6, SDLK_RALT},
{0xe7, SDLK_RSUPER},
};

int sdlInverseKeyMap[512];

void about()
{
	printf("Usage: oecli [options] file...\n");
	printf("Options:\n");
	printf("  -r <path>          Sets resource path\n");
	printf("  -s <samplerate>    Sets audio sample rate\n");
	printf("  -b <buffersize>    Sets audio buffer size\n");
	printf("  -c <channelnum>    Sets audio channel number\n");
	printf("  -i                 Enabled audio input\n");
	printf("  -w <width>         Set screen width\n");
	printf("  -h <height>        Set screen height\n");
	printf("  -f                 Set full screen\n");
	printf("  -u <updatefreq>    Set video update frequency\n");
}

Uint32 sdlCallback(Uint32 interval, void *param)
{
	SDL_Event event;
	SDL_UserEvent userevent;
	
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	
	event.type = SDL_USEREVENT;
	event.user = userevent;
	
	SDL_PushEvent(&event);
	
	return interval;
}

int sdlOpen(int width, int height, int fullscreen, double updateFrequency)
{
	SDL_Init(SDL_INIT_VIDEO |
			 SDL_INIT_TIMER |
			 SDL_INIT_JOYSTICK);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	SDL_Surface *videoSurface;
	videoSurface = SDL_SetVideoMode(width,
									height,
									0,
									SDL_OPENGL |
									(fullscreen ? SDL_FULLSCREEN : 0)
									);
	
	SDL_WM_SetCaption("OpenEmulator", NULL);
	
	int updateInterval = 1000.0 / updateFrequency;
	SDL_AddTimer(updateInterval, sdlCallback, NULL);
	
	for (int i = 0; i < sizeof(sdlKeyMap) / sizeof(SDLKeyMapEntry); i++)
		sdlInverseKeyMap[sdlKeyMap[i].sym] = sdlKeyMap[i].usbUsageId;
	
	return 0;
}

void sdlHandleKeyboardEvent(bool isKeyDown, SDL_keysym keysym)
{
	int scan = keysym.scancode;
	SDLKey sym = keysym.sym;
	
	int usbUsageId = 0;
	if (sym < 512)
		usbUsageId = sdlInverseKeyMap[sym];
	printf("kd:%d scan:%02x sym:%02x usb:%02x\n", isKeyDown, scan, sym, usbUsageId);
}

void sdlRunEventLoop()
{
	bool isRunning = true;
    SDL_Event event;
    while (isRunning && SDL_WaitEvent(&event))
	{
        switch(event.type)
		{
			case SDL_USEREVENT:
				oeglDraw();
				SDL_GL_SwapBuffers();
				break;				
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				sdlHandleKeyboardEvent(event.type == SDL_KEYDOWN,
									   event.key.keysym);
				break;
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				// SDL_WM_GrabInput(SDL_GRAB_ON)
				break;
			case SDL_JOYAXISMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				break;
			case SDL_QUIT:
				isRunning = false;
				break;
			default:
				break;
        }
    }
}

void sdlClose()
{
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	string resourcePath;
	int sdlWidth = 0;
	int sdlHeight = 0;
	bool sdlFullScreen = false;
	double sdlUpdateFrequency = 50.0;
	
	int c;
	
	while ((c = getopt(argc, argv, "r:s:b:c:iw:h:fu:")) != -1)
	{
		switch (c)
		{
			case 'r':
				resourcePath = string(optarg);
				break;
			case 's':
				oepaSetSampleRate(atoi(optarg));
				break;
			case 'b':
				oepaSetFramesPerBuffer(atoi(optarg));
				break;
			case 'c':
				oepaSetChannelNum(atoi(optarg));
				break;
			case 'i':
				oepaSetFullDuplex(true);
				break;
			case 'w':
				sdlWidth = atoi(optarg);
				break;
			case 'h':
				sdlHeight = atoi(optarg);
				break;
			case 'f':
				sdlFullScreen = true;
				break;
			case 'u':
				sdlUpdateFrequency = atof(optarg);
				break;
			case '?':
				about();
				return 0;
			default:
				printf("Unknown option '-%c'\n");
				return 0;				
		}
	}
	
	oepaOpen();
	
	// oepaConstruct
	// Determine host::video width and height
	sdlWidth = 640;
	sdlHeight = 480;
	
	sdlOpen(sdlWidth, sdlHeight, sdlFullScreen, sdlUpdateFrequency);
	oeglOpen();
	
	sdlRunEventLoop();
	
	oeglClose();
	sdlClose();
	oepaClose();
	
	return 0;
}
