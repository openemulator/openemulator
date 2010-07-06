
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

#include <sys/time.h>

#include <sstream>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "oepa.h"
#include "oegl.h"

#include "Host.h"

#define HOST_DEVICE "host::host"
#define SDL_INVERSE_KEYMAP_SIZE 512

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

OEEmulation *sdlEmulation;

int sdlInverseKeyMap[SDL_INVERSE_KEYMAP_SIZE];
bool sdlCtrlPressed;
bool sdlAltPressed;

vector<bool> sdlMouseButtonState;
bool sdlMouseCapture;
bool sdlMouseCaptured;
bool sdlMouseCaptureRelease;

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
	SDL_Event event = {SDL_USEREVENT};
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
	
	sdlCtrlPressed = false;
	sdlAltPressed = false;
	
	sdlMouseButtonState.resize(8);
	sdlMouseCaptured = false;
	sdlMouseCaptureRelease = false;
	
	return 0;
}

void sdlSendHIDEvent(int notification, int usageId, int value)
{
	HostHIDEvent hidEvent;
	hidEvent.usageId = usageId;
	hidEvent.value = value;
	oepaPostNotification(sdlEmulation, HOST_DEVICE,
						 notification, &hidEvent);
	
	printf("sendHIDEvent: %d, %d, %d\n", notification, usageId, value);
}

int sdlTranslateKeysym(int sym)
{
	return (sym < SDL_INVERSE_KEYMAP_SIZE) ? sdlInverseKeyMap[sym] : 0;
}

void sdlUpdateCtrlAlt(int sym, bool state)
{
	if ((sym == SDLK_LCTRL) || (sym == SDLK_RCTRL))
		sdlCtrlPressed = state;
	if ((sym == SDLK_LALT) || (sym == SDLK_RALT))
		sdlAltPressed = state;
}

void sdlSetGrabMode(bool state)
{
	SDL_WM_GrabInput(state ? SDL_GRAB_ON : SDL_GRAB_OFF);
	SDL_ShowCursor(state ? SDL_DISABLE : SDL_ENABLE);
					
	// Move mouse button events
	for (int i = 0; i < sdlMouseButtonState.size(); i++)
	{
		if (!sdlMouseButtonState[i])
			continue;
		
		if (state)
		{
			sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
							HOST_HID_P_BUTTON1 + i, false);
			sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
							HOST_HID_M_BUTTON1 + i, true);
		}
		else
		{
			sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
							HOST_HID_M_BUTTON1 + i, false);
			sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
							HOST_HID_P_BUTTON1 + i, true);
		}
	}
}

void sdlSendMouseButtonEvent(int button, bool state)
{
	if (sdlMouseButtonState[button] == state)
		return;
	
	sdlMouseButtonState[button] = state;
	
	if (sdlMouseCaptured)
		sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
						HOST_HID_M_BUTTON1 + button,
						state);
	else
		sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
						HOST_HID_P_BUTTON1 + button,
						state);
}

void sdlRunEventLoop()
{
    SDL_Event event;
	bool isRunning = true;
	int keyDownCount = 0;
	
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
				if ((event.key.keysym.unicode) &&
					(event.key.keysym.unicode < 0xf700) &&
					(event.key.keysym.unicode >= 0xf900))
					sdlSendHIDEvent(HOST_HID_UNICODEKEYBOARD_EVENT,
									event.key.keysym.unicode, 1);
				sdlSendHIDEvent(HOST_HID_KEYBOARD_EVENT,
								sdlTranslateKeysym(event.key.keysym.sym), 1);

				if ((event.key.keysym.sym == SDLK_CAPSLOCK) ||
					(event.key.keysym.sym == SDLK_NUMLOCK) ||
					(event.key.keysym.sym == SDLK_SCROLLOCK))
					sdlSendHIDEvent(HOST_HID_KEYBOARD_EVENT,
									sdlTranslateKeysym(event.key.keysym.sym), 0);
				else
					keyDownCount++;
				
				// CTRL-ALT releases mouse capture
				sdlUpdateCtrlAlt(event.key.keysym.sym, true);
				
				if (sdlMouseCaptured && sdlCtrlPressed && sdlAltPressed)
					sdlMouseCaptureRelease = true;
				break;
				
			case SDL_KEYUP:
				if ((event.key.keysym.sym == SDLK_CAPSLOCK) ||
					(event.key.keysym.sym == SDLK_NUMLOCK) ||
					(event.key.keysym.sym == SDLK_SCROLLOCK))
					sdlSendHIDEvent(HOST_HID_KEYBOARD_EVENT,
									sdlTranslateKeysym(event.key.keysym.sym), 1);
				else
					keyDownCount--;
				sdlSendHIDEvent(HOST_HID_KEYBOARD_EVENT,
								sdlTranslateKeysym(event.key.keysym.sym), 0);
				
				sdlUpdateCtrlAlt(event.key.keysym.sym, false);
				
				// Was mouse capture pressed, and no keys are down?
				if (sdlMouseCaptureRelease && !keyDownCount)
				{
					sdlMouseCaptured = false;
					sdlMouseCaptureRelease = false;
					sdlSetGrabMode(false);
				}
				break;
				
			case SDL_MOUSEMOTION:
				if (sdlMouseCaptured)
				{
					if (event.motion.xrel)
						sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
										HOST_HID_M_RX,
										event.motion.xrel);
					if (event.motion.yrel)
						sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
										HOST_HID_M_RY,
										event.motion.yrel);
				}
				else
				{
					sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
									HOST_HID_P_X,
									event.motion.x);
					sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
									HOST_HID_P_Y,
									event.motion.y);
				}
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				if ((event.button.button == SDL_BUTTON_WHEELUP) ||
					(event.button.button == SDL_BUTTON_WHEELDOWN))
				{
					int value = (event.button.button == SDL_BUTTON_WHEELUP) ? 1 : -1;
					if (sdlMouseCaptured)
						sdlSendHIDEvent(HOST_HID_MOUSE_EVENT,
										HOST_HID_M_WHEEL,
										value);
					else
						sdlSendHIDEvent(HOST_HID_POINTER_EVENT,
										HOST_HID_P_WHEEL,
										value);
				}
				else
				{
					if (sdlMouseCapture && !sdlMouseCaptured)
					{
						sdlMouseCaptured = true;
						sdlSetGrabMode(true);
						break;
					}
					
					sdlSendMouseButtonEvent(event.button.button, true);
				}
				break;
				
			case SDL_MOUSEBUTTONUP:
				if ((event.button.button != SDL_BUTTON_WHEELUP) &&
					(event.button.button != SDL_BUTTON_WHEELDOWN))
					sdlSendMouseButtonEvent(event.button.button, false);
				break;
				
			case SDL_JOYAXISMOTION:
				sdlSendHIDEvent(HOST_HID_JOYSTICK1_EVENT + event.jaxis.which,
								HOST_HID_J_AXIS1 + event.jaxis.axis,
								event.jaxis.value + 32768);
				break;
				
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				sdlSendHIDEvent(HOST_HID_JOYSTICK1_EVENT + event.jbutton.which,
								HOST_HID_J_BUTTON1 + event.jbutton.button,
								event.jbutton.state == SDL_PRESSED);
				break;
				
			case SDL_JOYHATMOTION:
				sdlSendHIDEvent(HOST_HID_JOYSTICK1_EVENT + event.jhat.which,
								HOST_HID_J_HAT1 + event.jhat.hat,
								event.jhat.value);
				break;
				
			case SDL_JOYBALLMOTION:
				sdlSendHIDEvent(HOST_HID_JOYSTICK1_EVENT + event.jball.which,
								HOST_HID_J_RELAXIS1 + 2 * event.jball.ball,
								event.jball.xrel);
				sdlSendHIDEvent(HOST_HID_JOYSTICK1_EVENT + event.jball.which,
								HOST_HID_J_RELAXIS2 + 2 * event.jball.ball,
								event.jball.yrel);
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
		
		while ((c = getopt(argc, argv, "r:s:b:c:iw:h:f")) != -1)
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
		
		if (oepaGetProperty(sdlEmulation, HOST_DEVICE, "hidMouseCapture", value) &&
			(value == "1"))
			sdlMouseCapture = true;
		
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
