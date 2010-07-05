
/**
 * OpenEmulator
 * CLI Main
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Program entry point.
 */

#include <sys/time.h>

#include "stdio.h"
#include "getopt.h"

#include <sstream>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "oepa.h"
#include "oegl.h"

#include "Host.h"

#define HOST_DEVICE "host::host"

typedef struct
{
	SDLKey sym;
	int usageId;
} SDLKeyMapEntry;

SDLKeyMapEntry sdlKeyMap[] = 
{
	{SDLK_a, HOST_HID_K_A},
	{SDLK_b, HOST_HID_K_B},
	{SDLK_c, HOST_HID_K_C},
	{SDLK_d, HOST_HID_K_D},
	{SDLK_e, HOST_HID_K_E},
	{SDLK_f, HOST_HID_K_F},
	{SDLK_g, HOST_HID_K_G},
	{SDLK_h, HOST_HID_K_H},
	{SDLK_i, HOST_HID_K_I},
	{SDLK_j, HOST_HID_K_J},
	{SDLK_k, HOST_HID_K_K},
	{SDLK_l, HOST_HID_K_L},
	{SDLK_m, HOST_HID_K_M},
	{SDLK_n, HOST_HID_K_N},
	{SDLK_o, HOST_HID_K_O},
	{SDLK_p, HOST_HID_K_P},
	{SDLK_q, HOST_HID_K_Q},
	{SDLK_r, HOST_HID_K_R},
	{SDLK_s, HOST_HID_K_S},
	{SDLK_t, HOST_HID_K_T},
	{SDLK_u, HOST_HID_K_U},
	{SDLK_v, HOST_HID_K_V},
	{SDLK_w, HOST_HID_K_W},
	{SDLK_x, HOST_HID_K_X},
	{SDLK_y, HOST_HID_K_Y},
	{SDLK_z, HOST_HID_K_Z},
	{SDLK_1, HOST_HID_K_1},
	{SDLK_2, HOST_HID_K_2},
	{SDLK_3, HOST_HID_K_3},
	{SDLK_4, HOST_HID_K_4},
	{SDLK_5, HOST_HID_K_5},
	{SDLK_6, HOST_HID_K_6},
	{SDLK_7, HOST_HID_K_7},
	{SDLK_8, HOST_HID_K_8},
	{SDLK_9, HOST_HID_K_9},
	{SDLK_0, HOST_HID_K_0},
	{SDLK_RETURN, HOST_HID_K_ENTER},
	{SDLK_ESCAPE, HOST_HID_K_ESCAPE},
	{SDLK_BACKSPACE, HOST_HID_K_BACKSPACE},
	{SDLK_TAB, HOST_HID_K_TAB},
	{SDLK_SPACE, HOST_HID_K_SPACE},
	{SDLK_MINUS, HOST_HID_K_MINUS},
	{SDLK_EQUALS, HOST_HID_K_EQUAL},
	{SDLK_LEFTBRACKET, HOST_HID_K_LEFTBRACKET},
	{SDLK_RIGHTBRACKET, HOST_HID_K_RIGHTBRACKET},
	{SDLK_BACKSLASH, HOST_HID_K_BACKSLASH},
	{SDLK_WORLD_0, HOST_HID_K_NON_US1},
	{SDLK_SEMICOLON, HOST_HID_K_SEMICOLON},
	{SDLK_QUOTE, HOST_HID_K_QUOTE},
	{SDLK_BACKQUOTE, HOST_HID_K_GRAVEACCENT},
	{SDLK_COMMA, HOST_HID_K_COMMA},
	{SDLK_PERIOD, HOST_HID_K_PERIOD},
	{SDLK_SLASH, HOST_HID_K_SLASH},
	{SDLK_CAPSLOCK, HOST_HID_K_CAPSLOCK},
	{SDLK_F1, HOST_HID_K_F1},
	{SDLK_F2, HOST_HID_K_F2},
	{SDLK_F3, HOST_HID_K_F3},
	{SDLK_F4, HOST_HID_K_F4},
	{SDLK_F5, HOST_HID_K_F5},
	{SDLK_F6, HOST_HID_K_F6},
	{SDLK_F7, HOST_HID_K_F7},
	{SDLK_F8, HOST_HID_K_F8},
	{SDLK_F9, HOST_HID_K_F9},
	{SDLK_F10, HOST_HID_K_F10},
	{SDLK_F11, HOST_HID_K_F11},
	{SDLK_F12, HOST_HID_K_F12},
	{SDLK_PRINT, HOST_HID_K_PRINTSCREEN},
	{SDLK_SCROLLOCK, HOST_HID_K_SCROLLLOCK},
	{SDLK_PAUSE, HOST_HID_K_PAUSE},
	{SDLK_INSERT, HOST_HID_K_INSERT},
	{SDLK_HOME, HOST_HID_K_HOME},
	{SDLK_PAGEUP, HOST_HID_K_PAGEUP},
	{SDLK_DELETE, HOST_HID_K_DELETE},
	{SDLK_END, HOST_HID_K_END},
	{SDLK_PAGEDOWN, HOST_HID_K_PAGEDOWN},
	{SDLK_RIGHT, HOST_HID_K_RIGHT},
	{SDLK_LEFT, HOST_HID_K_LEFT},
	{SDLK_DOWN, HOST_HID_K_DOWN},
	{SDLK_UP, HOST_HID_K_UP},
	{SDLK_NUMLOCK, HOST_HID_KP_NUMLOCK},
	{SDLK_KP_DIVIDE, HOST_HID_KP_SLASH},
	{SDLK_KP_MULTIPLY, HOST_HID_KP_STAR},
	{SDLK_KP_MINUS, HOST_HID_KP_MINUS},
	{SDLK_KP_PLUS, HOST_HID_KP_PLUS},
	{SDLK_KP_ENTER, HOST_HID_KP_ENTER},
	{SDLK_KP1, HOST_HID_KP_1},
	{SDLK_KP2, HOST_HID_KP_2},
	{SDLK_KP3, HOST_HID_KP_3},
	{SDLK_KP4, HOST_HID_KP_4},
	{SDLK_KP5, HOST_HID_KP_5},
	{SDLK_KP6, HOST_HID_KP_6},
	{SDLK_KP7, HOST_HID_KP_7},
	{SDLK_KP8, HOST_HID_KP_8},
	{SDLK_KP9, HOST_HID_KP_9},
	{SDLK_KP0, HOST_HID_KP_0},
	{SDLK_KP_PERIOD, HOST_HID_KP_PERIOD},
	{SDLK_F13, HOST_HID_K_F13},
	{SDLK_F14, HOST_HID_K_F14},
	{SDLK_F15, HOST_HID_K_F15},
	{SDLK_POWER, HOST_HID_K_POWER},
	{SDLK_HELP, HOST_HID_K_HELP},
	{SDLK_MENU, HOST_HID_K_MENU},
	{SDLK_UNDO, HOST_HID_K_UNDO},
	{SDLK_SYSREQ, HOST_HID_K_SYSREQ},
	{SDLK_LCTRL, HOST_HID_K_LEFTCONTROL},
	{SDLK_LSHIFT, HOST_HID_K_LEFTSHIFT},
	{SDLK_LALT, HOST_HID_K_LEFTALT},
	{SDLK_LSUPER, HOST_HID_K_LEFTGUI},
	{SDLK_LMETA, HOST_HID_K_LEFTGUI},
	{SDLK_RCTRL, HOST_HID_K_RIGHTCONTROL},
	{SDLK_RSHIFT, HOST_HID_K_RIGHTSHIFT},
	{SDLK_RALT, HOST_HID_K_RIGHTALT},
	{SDLK_MODE, HOST_HID_K_RIGHTALT},
	{SDLK_RSUPER, HOST_HID_K_RIGHTGUI},
	{SDLK_RMETA, HOST_HID_K_RIGHTGUI},
};

int sdlInverseKeyMap[512];
OEEmulation *sdlEmulation;
bool sdlMouseCapture;

void about()
{
	printf("Usage: oe [options] file...\n");
	printf("Options:\n");
	printf("  -r <path>          Sets resource path\n");
	printf("  -s <samplerate>    Sets audio sample rate\n");
	printf("  -b <buffersize>    Sets audio buffer size\n");
	printf("  -c <channelnum>    Sets audio channel number\n");
	printf("  -i                 Enable audio input\n");
	printf("  -w <width>         Set screen width\n");
	printf("  -h <height>        Set screen height\n");
	printf("  -f                 Set full screen\n");
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

int sdlOpen(int width, int height, int fullscreen)
{
	SDL_Init(SDL_INIT_VIDEO |
			 SDL_INIT_TIMER |
			 SDL_INIT_JOYSTICK);
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	SDL_Surface *videoSurface;
	videoSurface = SDL_SetVideoMode(width,
									height,
									0,
									SDL_OPENGL |
									(fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE)
									);
	SDL_WM_SetCaption("OpenEmulator", NULL);
	
	SDL_EnableUNICODE(1);
	for (int i = 0; i < sizeof(sdlKeyMap) / sizeof(SDLKeyMapEntry); i++)
		sdlInverseKeyMap[sdlKeyMap[i].sym] = sdlKeyMap[i].usageId;
	
	SDL_AddTimer(1, sdlCallback, NULL);
	return 0;
}

void sdlSendKeyboardEvent(bool isKeyDown, SDL_keysym keysym)
{
	int scan = keysym.scancode;
	SDLKey sym = keysym.sym;
	
	int usageId = 0;
	if (sym < 512)
		usageId = sdlInverseKeyMap[sym];
	
	if ((sym == SDLK_CAPSLOCK) || (sym == SDLK_NUMLOCK) || (sym == SDLK_SCROLLOCK))
	{
		isKeyDown = true;
		printf("kd:%d scan:%02x sym:%02x usb:%02x\n", isKeyDown, scan, sym, 
			   usageId);
		
		isKeyDown = false;
	}
	
	printf("kd:%d scan:%02x sym:%02x usb:%02x\n", isKeyDown, scan, sym, usageId);
}

void sdlSendJoystickAxisEvent(int which, int axis, int value)
{
	HostHIDEvent hidEvent;
	hidEvent.usageId = HOST_HID_J_AXIS1 + axis;
	hidEvent.value = value;
	oepaPostNotification(sdlEmulation, HOST_DEVICE,
						 HOST_HID_JOYSTICK1_EVENT + which,
						 &hidEvent);
}

void sdlSendJoystickButtonEvent(int which, int button, bool state)
{
	HostHIDEvent hidEvent;
	hidEvent.usageId = HOST_HID_J_BUTTON1 + button;
	hidEvent.value = state;
	oepaPostNotification(sdlEmulation, HOST_DEVICE,
						 HOST_HID_JOYSTICK1_EVENT + which,
						 &hidEvent);
}

void sdlRunEventLoop()
{
	bool isRunning = true;
    SDL_Event event;
	bool mouseCaptured = false;
	
    while (isRunning && SDL_WaitEvent(&event))
	{
        switch(event.type)
		{
			case SDL_USEREVENT:
			{
				static int last;
				timeval tim;
				gettimeofday(&tim, NULL);
				
//				printf("%d\n", (1000000 + tim.tv_usec - last) % 1000000);
				last = tim.tv_usec;
				
				oeglDraw(sdlEmulation);
				SDL_GL_SwapBuffers();
			}
				break;
				
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				sdlSendKeyboardEvent(event.type == SDL_KEYDOWN,
									 event.key.keysym);
				break;
				
			case SDL_MOUSEMOTION:
				if (!mouseCaptured)
				{
//					sendPointerPosition(x);
//					sendPointerPosition(y);
				}
				else
				{
//					if (rx)
//						sendMouseMotion(rx);
//					if (ry)
//						sendMouseMotion(ry);
				}
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				if (!mouseCaptured)
					if (sdlMouseCapture)
						SDL_WM_GrabInput(SDL_GRAB_ON);
//					else
//						sendPointerEvent();
//				else
//					sendMouseEvent();
				break;
				
			case SDL_MOUSEBUTTONUP:
//				if (mouseCaptured)
//					sendMouseEvent();
//				else if (mouseCapture)
//					sendPointerEvent();
				break;
				
			case SDL_JOYAXISMOTION:
				sdlSendJoystickAxisEvent(event.jaxis.which,
										 event.jaxis.axis,
										 event.jaxis.value);
				break;
				
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				sdlSendJoystickButtonEvent(event.jbutton.which,
										   event.jbutton.button,
										   event.jbutton.state == SDL_JOYBUTTONDOWN);
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
	string emulationPath;
	string resourcePath;

	int videoLeft = 0;
	int videoTop = 0;
	int videoWidth = 768;
	int videoHeight = 512;
	bool videoCanvasSizeSet = false;
	bool videoFullScreen = false;
	
	{
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
					videoWidth = atoi(optarg);
					videoCanvasSizeSet = true;
					break;
					
				case 'h':
					videoHeight = atoi(optarg);
					videoCanvasSizeSet = true;
					break;
					
				case 'f':
					videoFullScreen = true;
					break;
					
				case '?':
					about();
					return 0;
					
				default:
					printf("Unknown option '-%c'\n", c);
					return 0;				
			}
		}
	}
	
	if (optind == argc)
	{
		about();
		return 0;
	}
	
	int index = optind;
	if (index < argc)
		emulationPath = string(argv[index++]);
	
	oepaOpen();
	
	sdlEmulation = oepaConstruct(emulationPath, resourcePath);
	if (sdlEmulation && sdlEmulation->isLoaded())
	{
		// Configure emulation
		string value;
		if (oepaGetProperty(sdlEmulation, HOST_DEVICE, "videoWindow", value))
		{
			float x, y, w, h;
			if (sscanf(value.c_str(), "%f %f %f %f", &x, &y, &w, &h) == 4)
			{
				if (!videoCanvasSizeSet)
				{
					videoWidth = (int) w;
					videoHeight = (int) h;
				}
				
				videoLeft = (int) x;
				videoTop = (int) y;
			}
		}
		
		stringstream ss;
		ss << videoLeft << " " << videoTop << " " << videoWidth << " " << videoHeight;
		oepaSetProperty(sdlEmulation, HOST_DEVICE, "videoWindow", ss.str());
		
		oepaGetProperty(sdlEmulation, HOST_DEVICE, "mouseCapture", value);
		sdlMouseCapture = (value == "1") ? true : false;
		
		// To-Do: mount disk images
		
		// Open video
		sdlOpen(videoWidth, videoHeight, videoFullScreen);
		oeglInit(sdlEmulation);
		
		// Run emulation
		sdlRunEventLoop();
		
		// Close
		oepaSave(sdlEmulation, emulationPath);
		oepaDestroy(sdlEmulation);
		sdlClose();
	}
	else
		printf("Could not open emulation.\n");
	
	oepaClose();
	
	return 0;
}
