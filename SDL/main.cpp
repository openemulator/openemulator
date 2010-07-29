
/**
 * OpenEmulator
 * CLI Main
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Program entry point.
 */

#include "getopt.h"

#include <sstream>

#include "SDL/SDL.h"

#include "OEPA.h"
#include "OEGL.h"
#include "OEHID.h"

#include "Host.h"

#define sdlLog(text) cerr << "oecli: " << text << endl

#define SDL_KEYMAP_SIZE		512
#define SDL_KEYPRESSED_SIZE	512

typedef struct
{
	SDLKey sym;
	int usageId;
} SDLKeyMapInverseEntry;

SDLKeyMapInverseEntry sdlKeyMapInverse[] = 
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
	{SDLK_KP_EQUALS, HOST_HID_KP_EQUAL},
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

OEPAEmulation *sdlEmulation;
OEGL *sdlOEGL;
OEHID *sdlOEHID;

float sdlVideoWidth;
float sdlVideoHeight;
bool sdlVideoSync;

int sdlKeyMap[SDL_KEYMAP_SIZE];

void about()
{
	printf("Usage: oe [options] file...\n");
	printf("Options:\n");
	printf("  -r <path>            Sets resource path\n");
	printf("  -s <samplerate>      Sets audio sample rate\n");
	printf("  -b <buffersize>      Sets audio buffer size\n");
	printf("  -c <channelnum>      Sets audio channel number\n");
	printf("  -i                   Enable audio input\n");
	printf("  -w <width>           Set screen width\n");
	printf("  -h <height>          Set screen height\n");
	printf("  -f				   Set full screen\n");
	printf("  -k <keysym,usageid>  Set keymap entry (SDL keysym->USB usage id)\n");
}

bool sdlOpen(int width, int height, int fullscreen)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
		return false;
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	
	// SDL 1.2
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	// SDL 1.3
	//	SDL_GL_SetSwapInterval(1);
	
	sdlVideoWidth = width;
	sdlVideoHeight = height;
	
	int value;
	SDL_GL_GetAttribute(SDL_GL_SWAP_CONTROL, &value);
	sdlVideoSync = (value != 0);
	
	SDL_Surface *videoSurface;
	videoSurface = SDL_SetVideoMode(width,
									height,
									0,
									SDL_OPENGL |
									(fullscreen ? SDL_FULLSCREEN : 0)
									);
	if (!videoSurface)
		return false;
	
	SDL_WM_SetCaption("OpenEmulator", NULL);
	
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(500, 80);
	memset(sdlKeyMap, sizeof(sdlKeyMap), 0);
	for (int i = 0;
		 i < sizeof(sdlKeyMapInverse) / sizeof(SDLKeyMapInverseEntry);
		 i++)
	{
		int sym = sdlKeyMapInverse[i].sym;
		int usageId = sdlKeyMapInverse[i].usageId;
		sdlKeyMap[sym] = usageId;
	}
	
	return true;
}

void sdlSetKeyMapEntry(string translation)
{
	int sym = 0;
	int usageId = 0;
	
	int sepIndex = translation.find(',');
	if (sepIndex != string::npos)
	{
		sym = atoi(translation.c_str());
		usageId = atoi(translation.substr(sepIndex + 1).c_str());
	}
	
	if ((sym <= 0) || (sym >= SDL_KEYMAP_SIZE) || !usageId)
	{
		sdlLog("Invalid keymap entry '" << translation << "'");
		return;
	}
	
	sdlKeyMap[sym] = usageId;
}

int sdlGetUsageId(int keysym)
{
	int usageId = (keysym < SDL_KEYMAP_SIZE) ? sdlKeyMap[keysym] : 0;
	
	if (!usageId)
		sdlLog("unknown SDL keysym " << keysym);
	
	return usageId;
}

void sdlSetCapture(void *userData, bool value)
{
	SDL_WM_GrabInput(value ? SDL_GRAB_ON : SDL_GRAB_OFF);
	SDL_ShowCursor(value ? SDL_DISABLE : SDL_ENABLE);
}

void sdlRunEventLoop()
{
    SDL_Event event;
	bool running = true;
	
    while (running)
	{
		// Handle SDL events
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					// Ctrl-Backspace
					if ((event.key.keysym.mod & KMOD_CTRL) &&
						(event.key.keysym.sym == SDLK_BACKSPACE))
					{
						SDL_QuitEvent event = {SDL_QUIT};
						SDL_PushEvent((SDL_Event *) &event);
					}
					
					if (event.key.keysym.unicode)
						sdlOEHID->sendUnicode(event.key.keysym.unicode);
					
					sdlOEHID->setKey(sdlGetUsageId(event.key.keysym.sym), true);
					if ((event.key.keysym.sym == SDLK_CAPSLOCK) ||
						(event.key.keysym.sym == SDLK_NUMLOCK) ||
						(event.key.keysym.sym == SDLK_SCROLLOCK))
						sdlOEHID->setKey(sdlGetUsageId(event.key.keysym.sym), false);
					break;
					
				case SDL_KEYUP:
					if ((event.key.keysym.sym == SDLK_CAPSLOCK) ||
						(event.key.keysym.sym == SDLK_NUMLOCK) ||
						(event.key.keysym.sym == SDLK_SCROLLOCK))
						sdlOEHID->setKey(sdlGetUsageId(event.key.keysym.sym), true);
					sdlOEHID->setKey(sdlGetUsageId(event.key.keysym.sym), false);
					break;
					
				case SDL_MOUSEMOTION:
					sdlOEHID->setMousePosition(event.motion.x, event.motion.y);
					sdlOEHID->moveMouse(event.motion.xrel, event.motion.yrel);
					break;
					
				case SDL_MOUSEBUTTONDOWN:
					if ((event.button.button == SDL_BUTTON_WHEELUP) ||
						(event.button.button == SDL_BUTTON_WHEELDOWN))
					{
						float value = ((event.button.button == SDL_BUTTON_WHEELUP) ?
									   1 : -1);
						sdlOEHID->sendMouseWheelEvent(0, value);
						break;
					}
					
					if (event.button.button < 8)
					{
						int indexMap[] = {0, 0, 2, 1, 0, 0, 3, 4};
						int index = indexMap[event.button.button];
						
						sdlOEHID->setMouseButton(index, true);
					}
					break;
					
				case SDL_MOUSEBUTTONUP:
					if ((event.button.button == SDL_BUTTON_WHEELUP) ||
						(event.button.button == SDL_BUTTON_WHEELDOWN))
						break;
					
					if (event.button.button < 8)
					{
						int indexMap[] = {0, 0, 2, 1, 0, 0, 3, 4};
						int index = indexMap[event.button.button];
						
						sdlOEHID->setMouseButton(index, false);
					}
					break;
					
				case SDL_JOYAXISMOTION:
					sdlOEHID->setJoystickPosition(event.jaxis.which,
												  event.jaxis.axis,
												  event.jaxis.value);
					break;
					
				case SDL_JOYBALLMOTION:
					sdlOEHID->moveJoystickBall(event.jball.which,
											   event.jball.xrel,
											   event.jball.yrel);
					break;
					
				case SDL_JOYHATMOTION:
					sdlOEHID->sendJoystickHatEvent(event.jhat.which,
												   event.jhat.hat,
												   event.jhat.value);
					break;
					
				case SDL_JOYBUTTONDOWN:
					sdlOEHID->setJoystickButton(event.jbutton.which,
												event.jbutton.button,
												1);
					break;
					
				case SDL_JOYBUTTONUP:
					sdlOEHID->setJoystickButton(event.jbutton.which,
												event.jbutton.button,
												0);
					break;
					
				case SDL_QUIT:
					running = false;
					break;
					
				default:
					break;
			}
		}
		
		sdlOEGL->draw(sdlVideoWidth, sdlVideoHeight);
		SDL_GL_SwapBuffers();
		
		if (!sdlVideoSync)
			SDL_Delay(5);
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
	
	OEPA *oepa = new OEPA();
	if (!oepa)
	{
		sdlLog("could not open oepa");
		return 0;
	}
	
	{
		int c;
		
		while ((c = getopt(argc, argv, "r:s:b:c:iw:h:fk:")) != -1)
		{
			switch (c)
			{
				case 'r':
					resourcePath = string(optarg);
					break;
					
				case 's':
					oepa->setSampleRate(atoi(optarg));
					break;
					
				case 'b':
					oepa->setFramesPerBuffer(atoi(optarg));
					break;
					
				case 'c':
					oepa->setChannelNum(atoi(optarg));
					break;
					
				case 'i':
					oepa->setFullDuplex(true);
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
					
				case 'k':
					sdlSetKeyMapEntry(optarg);
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
	
	sdlEmulation = new OEPAEmulation(oepa, emulationPath, resourcePath);
	if (sdlEmulation && sdlEmulation->isLoaded())
	{
		sdlOEHID = new OEHID(sdlEmulation, sdlSetCapture);
		
		string value;
		if (sdlEmulation->getProperty(HOST_DEVICE, "videoWindow", value))
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
		sdlEmulation->setProperty(HOST_DEVICE, "videoWindow", ss.str());
		
		// To-Do: mount disk images
		
		// Open video
		if (sdlOpen(videoWidth, videoHeight, videoFullScreen))
		{
			sdlOEGL = new OEGL();
			
			oepa->open();
			
			// Run emulation
			sdlRunEventLoop();
			
			oepa->close();
			
			// Close
			sdlEmulation->save(emulationPath);

			delete sdlOEGL;
			sdlClose();
		}
		else
			sdlLog("coult not open sdl");
		
		delete sdlEmulation;
	}
	else
		sdlLog("could not open emulation");
	
	delete oepa;
	
	return 0;
}
