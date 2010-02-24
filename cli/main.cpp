
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

#include "HostKeyboard.h"

typedef struct
{
	SDLKey sym;
	int usbUsageId;
} SDLKeyMapEntry;

SDLKeyMapEntry sdlKeyMap[] = 
{
	{SDLK_a, HID_K_A},
	{SDLK_b, HID_K_B},
	{SDLK_c, HID_K_C},
	{SDLK_d, HID_K_D},
	{SDLK_e, HID_K_E},
	{SDLK_f, HID_K_F},
	{SDLK_g, HID_K_G},
	{SDLK_h, HID_K_H},
	{SDLK_i, HID_K_I},
	{SDLK_j, HID_K_J},
	{SDLK_k, HID_K_K},
	{SDLK_l, HID_K_L},
	{SDLK_m, HID_K_M},
	{SDLK_n, HID_K_N},
	{SDLK_o, HID_K_O},
	{SDLK_p, HID_K_P},
	{SDLK_q, HID_K_Q},
	{SDLK_r, HID_K_R},
	{SDLK_s, HID_K_S},
	{SDLK_t, HID_K_T},
	{SDLK_u, HID_K_U},
	{SDLK_v, HID_K_V},
	{SDLK_w, HID_K_W},
	{SDLK_x, HID_K_X},
	{SDLK_y, HID_K_Y},
	{SDLK_z, HID_K_Z},
	{SDLK_1, HID_K_1},
	{SDLK_2, HID_K_2},
	{SDLK_3, HID_K_3},
	{SDLK_4, HID_K_4},
	{SDLK_5, HID_K_5},
	{SDLK_6, HID_K_6},
	{SDLK_7, HID_K_7},
	{SDLK_8, HID_K_8},
	{SDLK_9, HID_K_9},
	{SDLK_0, HID_K_0},
	{SDLK_RETURN, HID_K_ENTER},
	{SDLK_ESCAPE, HID_K_ESCAPE},
	{SDLK_BACKSPACE, HID_K_BACKSPACE},
	{SDLK_TAB, HID_K_TAB},
	{SDLK_SPACE, HID_K_SPACE},
	{SDLK_MINUS, HID_K_MINUS},
	{SDLK_EQUALS, HID_K_EQUAL},
	{SDLK_LEFTBRACKET, HID_K_LEFTBRACKET},
	{SDLK_RIGHTBRACKET, HID_K_RIGHTBRACKET},
	{SDLK_BACKSLASH, HID_K_BACKSLASH},
	{SDLK_WORLD_0, HID_K_NON_US1},
	{SDLK_SEMICOLON, HID_K_SEMICOLON},
	{SDLK_QUOTE, HID_K_QUOTE},
	{SDLK_BACKQUOTE, HID_K_GRAVEACCENT},
	{SDLK_COMMA, HID_K_COMMA},
	{SDLK_PERIOD, HID_K_PERIOD},
	{SDLK_SLASH, HID_K_SLASH},
	{SDLK_CAPSLOCK, HID_K_CAPSLOCK},
	{SDLK_F1, HID_K_F1},
	{SDLK_F2, HID_K_F2},
	{SDLK_F3, HID_K_F3},
	{SDLK_F4, HID_K_F4},
	{SDLK_F5, HID_K_F5},
	{SDLK_F6, HID_K_F6},
	{SDLK_F7, HID_K_F7},
	{SDLK_F8, HID_K_F8},
	{SDLK_F9, HID_K_F9},
	{SDLK_F10, HID_K_F10},
	{SDLK_F11, HID_K_F11},
	{SDLK_F12, HID_K_F12},
	
	{SDLK_SCROLLOCK, HID_K_SCROLLLOCK},
	{SDLK_PAUSE, HID_K_PAUSE},
	{SDLK_INSERT, HID_K_INSERT},
	{SDLK_HOME, HID_K_HOME},
	{SDLK_PAGEUP, HID_K_PAGEUP},
	{SDLK_DELETE, HID_K_DELETE},
	{SDLK_END, HID_K_END},
	{SDLK_PAGEDOWN, HID_K_PAGEDOWN},
	{SDLK_RIGHT, HID_K_RIGHT},
	{SDLK_LEFT, HID_K_LEFT},
	{SDLK_DOWN, HID_K_DOWN},
	{SDLK_UP, HID_K_UP},
	
	{SDLK_NUMLOCK, HID_KP_NUMLOCK},
	{SDLK_KP_DIVIDE, HID_KP_SLASH},
	{SDLK_KP_MULTIPLY, HID_KP_STAR},
	{SDLK_KP_MINUS, HID_KP_MINUS},
	{SDLK_KP_PLUS, HID_KP_PLUS},
	{SDLK_KP_ENTER, HID_KP_ENTER},
	{SDLK_KP1, HID_KP_1},
	{SDLK_KP2, HID_KP_2},
	{SDLK_KP3, HID_KP_3},
	{SDLK_KP4, HID_KP_4},
	{SDLK_KP5, HID_KP_5},
	{SDLK_KP6, HID_KP_6},
	{SDLK_KP7, HID_KP_7},
	{SDLK_KP8, HID_KP_8},
	{SDLK_KP9, HID_KP_9},
	{SDLK_KP0, HID_KP_0},
	{SDLK_KP_PERIOD, HID_KP_PERIOD},
	
	{SDLK_F13, HID_K_F13},
	{SDLK_F14, HID_K_F13},
	{SDLK_F15, HID_K_F13},
	
	{SDLK_LCTRL, HID_K_LEFTCONTROL},
	{SDLK_LSHIFT, HID_K_LEFTSHIFT},
	{SDLK_LALT, HID_K_LEFTALT},
	{SDLK_LSUPER, HID_K_LEFTGUI},
	{SDLK_LMETA, HID_K_LEFTGUI},
	{SDLK_RCTRL, HID_K_RIGHTCONTROL},
	{SDLK_RSHIFT, HID_K_RIGHTSHIFT},
	{SDLK_RALT, HID_K_RIGHTALT},
	{SDLK_MODE, HID_K_RIGHTALT},
	{SDLK_RSUPER, HID_K_RIGHTGUI},		
	{SDLK_RMETA, HID_K_RIGHTGUI},		
};

int sdlKeyTable[512];

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
		sdlKeyTable[sdlKeyMap[i].sym] = sdlKeyMap[i].usbUsageId;
	
	return 0;
}

void sdlHandleKeyboardEvent(bool isKeyDown, SDL_keysym keysym)
{
	int scan = keysym.scancode;
	SDLKey sym = keysym.sym;
	
	int usbUsageId = 0;
	if (sym < 512)
		usbUsageId = sdlKeyTable[sym];
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
