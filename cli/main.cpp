
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

#include "HostHID.h"

typedef struct
{
	SDLKey sym;
	int usbUsageId;
} SDLKeyMapEntry;

SDLKeyMapEntry sdlKeyMap[] = 
{
	{SDLK_a, HOSTHID_K_A},
	{SDLK_b, HOSTHID_K_B},
	{SDLK_c, HOSTHID_K_C},
	{SDLK_d, HOSTHID_K_D},
	{SDLK_e, HOSTHID_K_E},
	{SDLK_f, HOSTHID_K_F},
	{SDLK_g, HOSTHID_K_G},
	{SDLK_h, HOSTHID_K_H},
	{SDLK_i, HOSTHID_K_I},
	{SDLK_j, HOSTHID_K_J},
	{SDLK_k, HOSTHID_K_K},
	{SDLK_l, HOSTHID_K_L},
	{SDLK_m, HOSTHID_K_M},
	{SDLK_n, HOSTHID_K_N},
	{SDLK_o, HOSTHID_K_O},
	{SDLK_p, HOSTHID_K_P},
	{SDLK_q, HOSTHID_K_Q},
	{SDLK_r, HOSTHID_K_R},
	{SDLK_s, HOSTHID_K_S},
	{SDLK_t, HOSTHID_K_T},
	{SDLK_u, HOSTHID_K_U},
	{SDLK_v, HOSTHID_K_V},
	{SDLK_w, HOSTHID_K_W},
	{SDLK_x, HOSTHID_K_X},
	{SDLK_y, HOSTHID_K_Y},
	{SDLK_z, HOSTHID_K_Z},
	{SDLK_1, HOSTHID_K_1},
	{SDLK_2, HOSTHID_K_2},
	{SDLK_3, HOSTHID_K_3},
	{SDLK_4, HOSTHID_K_4},
	{SDLK_5, HOSTHID_K_5},
	{SDLK_6, HOSTHID_K_6},
	{SDLK_7, HOSTHID_K_7},
	{SDLK_8, HOSTHID_K_8},
	{SDLK_9, HOSTHID_K_9},
	{SDLK_0, HOSTHID_K_0},
	{SDLK_RETURN, HOSTHID_K_ENTER},
	{SDLK_ESCAPE, HOSTHID_K_ESCAPE},
	{SDLK_BACKSPACE, HOSTHID_K_BACKSPACE},
	{SDLK_TAB, HOSTHID_K_TAB},
	{SDLK_SPACE, HOSTHID_K_SPACE},
	{SDLK_MINUS, HOSTHID_K_MINUS},
	{SDLK_EQUALS, HOSTHID_K_EQUAL},
	{SDLK_LEFTBRACKET, HOSTHID_K_LEFTBRACKET},
	{SDLK_RIGHTBRACKET, HOSTHID_K_RIGHTBRACKET},
	{SDLK_BACKSLASH, HOSTHID_K_BACKSLASH},
	{SDLK_WORLD_0, HOSTHID_K_NON_US1},
	{SDLK_SEMICOLON, HOSTHID_K_SEMICOLON},
	{SDLK_QUOTE, HOSTHID_K_QUOTE},
	{SDLK_BACKQUOTE, HOSTHID_K_GRAVEACCENT},
	{SDLK_COMMA, HOSTHID_K_COMMA},
	{SDLK_PERIOD, HOSTHID_K_PERIOD},
	{SDLK_SLASH, HOSTHID_K_SLASH},
	{SDLK_CAPSLOCK, HOSTHID_K_CAPSLOCK},
	{SDLK_F1, HOSTHID_K_F1},
	{SDLK_F2, HOSTHID_K_F2},
	{SDLK_F3, HOSTHID_K_F3},
	{SDLK_F4, HOSTHID_K_F4},
	{SDLK_F5, HOSTHID_K_F5},
	{SDLK_F6, HOSTHID_K_F6},
	{SDLK_F7, HOSTHID_K_F7},
	{SDLK_F8, HOSTHID_K_F8},
	{SDLK_F9, HOSTHID_K_F9},
	{SDLK_F10, HOSTHID_K_F10},
	{SDLK_F11, HOSTHID_K_F11},
	{SDLK_F12, HOSTHID_K_F12},
	{SDLK_PRINT, HOSTHID_K_PRINTSCREEN},
	{SDLK_SCROLLOCK, HOSTHID_K_SCROLLLOCK},
	{SDLK_PAUSE, HOSTHID_K_PAUSE},
	{SDLK_INSERT, HOSTHID_K_INSERT},
	{SDLK_HOME, HOSTHID_K_HOME},
	{SDLK_PAGEUP, HOSTHID_K_PAGEUP},
	{SDLK_DELETE, HOSTHID_K_DELETE},
	{SDLK_END, HOSTHID_K_END},
	{SDLK_PAGEDOWN, HOSTHID_K_PAGEDOWN},
	{SDLK_RIGHT, HOSTHID_K_RIGHT},
	{SDLK_LEFT, HOSTHID_K_LEFT},
	{SDLK_DOWN, HOSTHID_K_DOWN},
	{SDLK_UP, HOSTHID_K_UP},
	{SDLK_NUMLOCK, HOSTHID_KP_NUMLOCK},
	{SDLK_KP_DIVIDE, HOSTHID_KP_SLASH},
	{SDLK_KP_MULTIPLY, HOSTHID_KP_STAR},
	{SDLK_KP_MINUS, HOSTHID_KP_MINUS},
	{SDLK_KP_PLUS, HOSTHID_KP_PLUS},
	{SDLK_KP_ENTER, HOSTHID_KP_ENTER},
	{SDLK_KP1, HOSTHID_KP_1},
	{SDLK_KP2, HOSTHID_KP_2},
	{SDLK_KP3, HOSTHID_KP_3},
	{SDLK_KP4, HOSTHID_KP_4},
	{SDLK_KP5, HOSTHID_KP_5},
	{SDLK_KP6, HOSTHID_KP_6},
	{SDLK_KP7, HOSTHID_KP_7},
	{SDLK_KP8, HOSTHID_KP_8},
	{SDLK_KP9, HOSTHID_KP_9},
	{SDLK_KP0, HOSTHID_KP_0},
	{SDLK_KP_PERIOD, HOSTHID_KP_PERIOD},
	{SDLK_F13, HOSTHID_K_F13},
	{SDLK_F14, HOSTHID_K_F14},
	{SDLK_F15, HOSTHID_K_F15},
	{SDLK_POWER, HOSTHID_K_POWER},
	{SDLK_HELP, HOSTHID_K_HELP},
	{SDLK_MENU, HOSTHID_K_MENU},
	{SDLK_UNDO, HOSTHID_K_UNDO},
	{SDLK_SYSREQ, HOSTHID_K_SYSREQ},
	{SDLK_LCTRL, HOSTHID_K_LEFTCONTROL},
	{SDLK_LSHIFT, HOSTHID_K_LEFTSHIFT},
	{SDLK_LALT, HOSTHID_K_LEFTALT},
	{SDLK_LSUPER, HOSTHID_K_LEFTGUI},
	{SDLK_LMETA, HOSTHID_K_LEFTGUI},
	{SDLK_RCTRL, HOSTHID_K_RIGHTCONTROL},
	{SDLK_RSHIFT, HOSTHID_K_RIGHTSHIFT},
	{SDLK_RALT, HOSTHID_K_RIGHTALT},
	{SDLK_MODE, HOSTHID_K_RIGHTALT},
	{SDLK_RSUPER, HOSTHID_K_RIGHTGUI},
	{SDLK_RMETA, HOSTHID_K_RIGHTGUI},
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
	printf("  -i                 Enable audio input\n");
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
	
	if ((sym == SDLK_CAPSLOCK) || (sym == SDLK_NUMLOCK) || (sym == SDLK_SCROLLOCK))
	{
		isKeyDown = true;
		printf("kd:%d scan:%02x sym:%02x usb:%02x\n", isKeyDown, scan, sym, 
			   usbUsageId);
		
		isKeyDown = false;
	}
		
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
				printf("Unknown option '-%c'\n", c);
				return 0;				
		}
	}
	
	oepaOpen();
	
	// oepaConstruct
	// Determine host::video width and height
	sdlWidth = 768;
	sdlHeight = 576;
	
	sdlOpen(sdlWidth, sdlHeight, sdlFullScreen, sdlUpdateFrequency);
	oeglOpen();
	
	sdlRunEventLoop();
	
	oeglClose();
	sdlClose();
	oepaClose();
	
	return 0;
}
