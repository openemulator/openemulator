
/**
 * AppleIIGo
 * SDL IO class
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <iostream>

#include "SDLIO.h"
#include "SDLIOMessages.h"
#include "SDLMain.h"

using namespace std;

int SDLIO::runSdlThread(void * userdata)
{
	((SDLIO *) userdata)->runThread();
	return 0;
}

void SDLIO::callbackSdl(void * userdata, uint8_t * stream, int len)
{
	((SDLIO *) userdata)->callbackAudio((int16_t * ) stream, len >> 2);
}

uint32_t keypadSym[] =
{
	SDLK_KP0, SDLK_KP1, SDLK_KP2, SDLK_KP3, SDLK_KP4,
	SDLK_KP5, SDLK_KP6, SDLK_KP7, SDLK_KP8, SDLK_KP9,
	SDLK_KP_PERIOD, SDLK_KP_DIVIDE, SDLK_KP_MULTIPLY,
	SDLK_KP_MINUS, SDLK_KP_PLUS, SDLK_KP_ENTER, SDLK_KP_EQUALS,
};

SDLIO::SDLIO(map<string, string> &parameters)
{
	initThread();
	initAudio();
	initVideo();
	initKeyboard();
	initJoystick();
	initMouse();
}

SDLIO::~SDLIO()
{
}

void SDLIO::sendSdlIoMessage(uint32_t messageIndex, void * messageData)
{
	switch (messageIndex)
	{
		case SDLIOMESSAGE_QUIT:
			sendQuitEvent();
			break;
		case SDLIOMESSAGE_TOGGLEFULLSCREEN:
			toggleFullscreen();
			break;
	}
}

void SDLIO::sendComponentMessage(string &destination, void * messageData)
{

}

bool SDLIO::open() 
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return false;
    }
	
	openJoystick();
	openMouse();
	openKeyboard();
	
	if (!openVideo("AppleIIGo"))
	{
		fprintf(stderr, "Unable to open SDL video: %s\n", SDL_GetError());
		return false;
	}
	
	if (!openAudio())
	{
		fprintf(stderr, "Unable to open SDL audio: %s\n", SDL_GetError());
		return false;
	}
	
	if (!openThread())
	{
		fprintf(stderr, "Unable to open SDL thread: %s\n", SDL_GetError());
		return false;
	}
	
    SDL_PauseAudio(0);
	
	return true;
}

void SDLIO::close()
{
	SDL_PauseAudio(1);
	
	closeThread();
	closeAudio();
	closeVideo();
	closeKeyboard();
	closeMouse();
	closeJoystick();
	
	SDL_Quit();
}

void SDLIO::lock()
{
	if (!mutex)
		return;
	
	if (SDL_mutexP(mutex) == -1)
	{
		fprintf(stderr, "Unable to lock SDL mutex: %s\n", SDL_GetError());
		exit(-1);
	}
}

void SDLIO::unlock()
{
	if (!mutex)
		return;
	
	if (SDL_mutexV(mutex) == -1)
	{
		fprintf(stderr, "Unable to unlock SDL mutex: %s\n", SDL_GetError());
		exit(-1);
	}
}

void SDLIO::initThread()
{
	mutex = NULL;
	cond = NULL;
	thread = NULL;
	
	isThreadPausedValue = false;
}

bool SDLIO::openThread()
{
	bool isError = false;
	
	if (!isError)
		isError = (mutex = SDL_CreateMutex()) == NULL;
	if (!isError)
		isError = (cond = SDL_CreateCond()) == NULL;	
	if (!isError)
	{
		isKillThreadSignal = false;
		thread = SDL_CreateThread(SDLIO::runSdlThread, this);
		isError = (thread == NULL);
	}
	
	if (isError)
	{
		closeThread();
		return false;
	}
	
	return true;
}

void SDLIO::closeThread()
{
	if (thread)
	{
		lock();
		isKillThreadSignal = true;
		SDL_CondSignal(cond);
		unlock();
		SDL_WaitThread(thread, NULL);
	}
	thread = NULL;
	
	if (cond)
		SDL_DestroyCond(cond);
	cond = NULL;
	if (mutex)
		SDL_DestroyMutex(mutex);
	mutex = NULL;
}

void SDLIO::setPause(bool value)
{
	isThreadPausedValue = value;
}

bool SDLIO::isPaused()
{
	return isThreadPausedValue;
}

void SDLIO::runThread()
{
	lock();
	while (!isKillThreadSignal)
	{
		if (SDL_CondWait(cond, mutex) == -1)
		{
			fprintf(stderr, "Unable to wait for SDL condition: %s\n",
					SDL_GetError());
			exit(-1);
		}
		
		// To-Do: Add NTSC/PAL change
/*		if (isPalTiming != motherboard->getTimingGenerator()->isPalTiming())
		{
			closeAudio();
			openAudio();
		}
*/		
		if (SDL_MUSTLOCK(videoSurface))
			if (SDL_LockSurface(videoSurface) < 0)
			{
				fprintf(stderr, "Unable to lock SDL surface: %s\n",
						SDL_GetError());
				exit(-1);
			}
		
/*		motherboard->getVideoRenderer()->setFramebuffer((uint32_t *)
														videoSurface->pixels,
														videoSurface->pitch);
		if (!isThreadPaused())
		{
			// To-Do: When sampleBuffer is larger than number of samples
			// in a frame
			// call multiple times, move the extra part to the beginning
			motherboard->processFrame();
		}
 */
		
		if (SDL_MUSTLOCK(videoSurface))
			SDL_UnlockSurface(videoSurface);
		
/*		if (motherboard->getVideoRenderer()->isFramebufferUpdated())
		{
			motherboard->getVideoRenderer()->setIsFramebufferUpdated(false);
			SDL_Flip(videoSurface);
		}*/
	}
	unlock();
}

void SDLIO::initAudio()
{
//	currentAudioVolume = audioVolume = 1.0;
}

bool SDLIO::openAudio()
{
	SDL_AudioSpec audioFormat;
	bool isError = false;
	
	audioInputBuffer = new int32_t[SDLIO_AUDIO_SAMPLENUM];
	audioOutputBuffer = new int32_t[SDLIO_AUDIO_SAMPLENUM];
	
/*	audioFrameSampleNum = (SDLIO_AUDIO_SAMPLERATE /
						   motherboard->getTimingGenerator()->getFrameRate());*/
	audioFrameSampleNum = 1024;
	int16_t cpuByteOrderTestValue = 1;
	
	audioFormat.freq = SDLIO_AUDIO_SAMPLERATE;
	audioFormat.format = (*((char *) &cpuByteOrderTestValue) == 1) ?
		AUDIO_S16LSB : AUDIO_S16MSB;
	audioFormat.channels = 2;
	audioFormat.samples = audioFrameSampleNum;
	audioFormat.callback = SDLIO::callbackSdl;
	audioFormat.userdata = this;
	
	isError = (SDL_OpenAudio(&audioFormat, NULL) != 0);
	
	if (isError)
	{
		closeAudio();
		return false;
	}
	
/*	motherboard->setAudioConfiguration(audioInputBuffer,
									   audioOutputBuffer,
									   audioFrameSampleNum);*/
	
	return true;
}

void SDLIO::closeAudio()
{
	SDL_CloseAudio();
	
	delete audioInputBuffer;
	delete audioOutputBuffer;
}

void SDLIO::callbackAudio(int16_t * sampleData, uint32_t audioBufferSampleNum)
{
	lock();
	
	if (audioBufferSampleNum != audioFrameSampleNum)
		printf("ERR: audioBufferSampleNum<>audioFrameSampleNum (%d vs. %d)\n", 
			   audioBufferSampleNum, audioFrameSampleNum);
	
/*	int32_t * inputBuffer = audioInputBuffer;
	int32_t * outputBuffer = audioOutputBuffer;
	for (uint32_t index = 0; index < audioBufferSampleNum; index++)
	{
		if (currentAudioVolume != audioVolume)
			currentAudioVolume = currentAudioVolume *
				(1.0F - SDLIO_AUDIO_VOLUMEFILTERFACTOR)
				+ audioVolume * SDLIO_AUDIO_VOLUMEFILTERFACTOR;
		
		*inputBuffer++ = *sampleData;
		*sampleData++ = saturateAudio((int32_t) (*outputBuffer *
												currentAudioVolume));
		*outputBuffer++ = 0;
		
		*inputBuffer++ = *sampleData;
		*sampleData++ = saturateAudio((int32_t) (*outputBuffer *
												currentAudioVolume));
		*outputBuffer++ = 0;
	}*/
	unlock();
	SDL_CondSignal(cond);
}

void SDLIO::initVideo()
{
	isFullscreenValue = false;
	videoSurface = NULL;
	videoCaption = "";
}

bool SDLIO::openVideo(const char * windowCaption)
{
	bool isError = false;
	
	if (!isError)
		isError = (videoSurface =
				   SDL_SetVideoMode(SDLIO_VIDEO_X, SDLIO_VIDEO_Y, 32,
									SDL_HWSURFACE |
									(isFullscreen() ? SDL_FULLSCREEN : 0))
		) == NULL;
	
	if (!isError)
	{
		if (windowCaption)
			videoCaption = windowCaption;
		SDL_WM_SetCaption(videoCaption, NULL);
	}
	
	if (isError)
		closeVideo();
	
	return !isError;
}

void SDLIO::closeVideo()
{
}

void SDLIO::setFullscreen(bool value)
{
	isFullscreenValue = value;
	sendReopenEvent();
}

void SDLIO::toggleFullscreen()
{
	setFullscreen(!isFullscreenValue);
}

bool SDLIO::isFullscreen()
{
	return isFullscreenValue;
}

void SDLIO::initKeyboard()
{
	resetCount = 0;
}

void SDLIO::openKeyboard()
{
	SDL_EnableKeyRepeat(500, 100);
	SDL_EnableUNICODE(1);
	
	keyDownList.clear();
}

void SDLIO::closeKeyboard()
{
	while (!keyDownList.empty())
		pullKeyboardKey(keyDownList.front(), 0, 0);
}

void SDLIO::pushKeyboardKey(uint32_t sym, uint32_t mod, uint32_t unicode)
{
	lock();
	
	if (!isFullscreen() &&
		((((sym == SDLK_LCTRL) || (sym == SDLK_RCTRL)) && (mod & KMOD_ALT)) ||
		 (((sym == SDLK_LALT) || (sym == SDLK_RALT)) && (mod & KMOD_CTRL))))
		releaseMouse();
	
	switch (sym)
	{
		case SDLK_SPACE:
			if (mod & KMOD_CTRL)
				setPause(!isPaused());
			break;
		case SDLK_BACKSPACE:
			if (mod & KMOD_CTRL)
			{
				if (mod & KMOD_META)
				{
					if (mod & KMOD_ALT)
						sendQuitEvent();
					else
					{
/*						if (!resetCount)
						{
							motherboard->getCpu()->assertReset(true);
							resetCount++;
						}*/
					}
				}
				else
				{
/*					if (!resetCount)
					{
						motherboard->getCpu()->assertReset(false);
						resetCount++;
					}*/
				}
				unicode = 0;
			}
			else
				unicode = 8;
			break;
		case SDLK_RETURN:
			if (mod & (KMOD_META | KMOD_ALT) && !(mod & KMOD_SHIFT))
			{
				toggleFullscreen();
				unicode = 0;
			}
			break;
		case SDLK_LEFT:
			unicode = 8;
			break;
		case SDLK_RIGHT:
			unicode = 21;
			break;
		case SDLK_UP:
			unicode = 11;
			break;
		case SDLK_DOWN:
			unicode = 10;
			break;
		case SDLK_DELETE:
			unicode = 127;
			break;
		case SDLK_F4:
//			motherboard->getKeyboard()->toggleIIc40Switch();
			break;
		case SDLK_F7:
//			motherboard->getCpu()->triggerNmi();
			break;
	}
	
//	if (unicode > 0)
//		motherboard->getKeyboard()->pushKey(unicode);
	
	keyDownList.push_front(sym);
	
//	motherboard->getKeyboard()->pushKeyModifier(getKeyboardModifierIndex(sym));
//	motherboard->getKeyboard()->setAnyKeyDown(keyDownList->isAnyKeyDown());
	
	unlock();
}

void SDLIO::pullKeyboardKey(uint32_t sym, uint32_t mod, uint32_t unicode)
{
	lock();
	
	if (sym == SDLK_BACKSPACE)
	{
		if (resetCount)
		{
//			motherboard->getCpu()->clearReset();
			resetCount--;
		}
	}
	
	keyDownList.remove(sym);
	
//	motherboard->getKeyboard()->pullKeyModifier(getKeyboardModifierIndex(sym));
//	motherboard->getKeyboard()->setAnyKeyDown(keyDownList->isAnyKeyDown());
	
	unlock();
}

uint32_t SDLIO::getKeyboardModifierIndex(uint32_t sym)
{
/*	if ((sym == SDLK_LSHIFT) || (sym == SDLK_RSHIFT))
		return KEYMODIFIER_SHIFT;
	if ((sym == SDLK_LCTRL) || (sym == SDLK_RCTRL))
		return KEYMODIFIER_CONTROL;
	if (sym == SDLK_CAPSLOCK)
		return KEYMODIFIER_CAPSLOCK;
	if ((sym == SDLK_LALT) || (sym == SDLK_RALT))
		return KEYMODIFIER_OPTION;
	if ((sym == SDLK_LMETA) || (sym == SDLK_RMETA))
		return KEYMODIFIER_COMMAND;
	
	for (int symIndex = 0; symIndex < sizeof(keypadSym) / sizeof(uint32_t);
		 symIndex++)
	{
		if (sym == keypadSym[symIndex])
			return KEYMODIFIER_KEYPAD;
	}
	
	return KEYMODIFIER_END;*/
	return 0;
}

void SDLIO::initMouse()
{
	for (uint32_t index = 0; index < MOUSEBUTTON_NUM; index++)
		isMouseButtonDown[index] = false;
}

void SDLIO::openMouse()
{
	isMouseCapturedValue = false;
	isFirstMouseMovementSkipped = false;
	
	if (isFullscreen())
		captureMouse();
	else
		releaseMouse();
}

void SDLIO::closeMouse()
{
	releaseMouse();
}

void SDLIO::moveMouse(int32_t xRelative, int32_t yRelative)
{
	if (!isFirstMouseMovementSkipped)
	{
		isFirstMouseMovementSkipped = true;
		return;
	}
	
	lock();

//	motherboard->getMouse()->moveMouse(xRelative, yRelative);
	
	unlock();
}

void SDLIO::pushMouseButton(uint32_t button)
{
	lock();
	
	uint32_t index = getMouseButtonIndex(button);
	if (index < MOUSEBUTTON_NUM)
	{
		if (!isMouseButtonDown[index])
		{
			isMouseButtonDown[index] = true;
//			motherboard->getMouse()->pushButton(index);
		}
	}
	
	unlock();
}

void SDLIO::pullMouseButton(uint32_t button)
{
	lock();
	
	uint32_t index = getMouseButtonIndex(button);
	if (index < MOUSEBUTTON_NUM)
	{
		if (isMouseButtonDown[index])
		{
			isMouseButtonDown[index] = false;
//			motherboard->getMouse()->pullButton(index);
		}
	}
	
	unlock();
}

void SDLIO::clearMouseButtons()
{
	lock();
	
	for (uint32_t index = 0; index < MOUSEBUTTON_NUM; index++)
		pullMouseButton(index);
	
	unlock();
}

void SDLIO::captureMouse()
{
	if (!isMouseCaptured())
	{
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(false);
		isMouseCapturedValue = true;
		isFirstMouseMovementSkipped = false;
	}
}

void SDLIO::releaseMouse()
{
	if (isMouseCaptured())
	{
		clearMouseButtons();
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(true);		
		isMouseCapturedValue = false;
	}
}

bool SDLIO::isMouseCaptured()
{
	return isMouseCapturedValue;
}

uint32_t SDLIO::getMouseButtonIndex(uint32_t sdlButtonIndex)
{
	switch (sdlButtonIndex)
	{
		case SDL_BUTTON_LEFT:
			return 0;
		case SDL_BUTTON_RIGHT:
			return 1;
		case SDL_BUTTON_MIDDLE:
			return 2;
		default:
			return 3;
	}
}

void SDLIO::initJoystick()
{
	joystick = NULL;
}

void SDLIO::openJoystick()
{
	joystick = (SDL_NumJoysticks() > 0) ? SDL_JoystickOpen(0) : NULL;
}

void SDLIO::closeJoystick()
{
	if (joystick)
		SDL_JoystickClose(joystick);
}

void SDLIO::setJoystickPosition(uint32_t axis, uint32_t value)
{
//	motherboard->getPaddles()->setPosition(axis, value);
}

void SDLIO::pushJoystickButton(uint32_t button)
{
	lock();
	
//	motherboard->getPaddles()->pushButton(button);
	
	unlock();
}

void SDLIO::pullJoystickButton(uint32_t button)
{
	lock();
	
//	motherboard->getPaddles()->pullButton(button);
	
	unlock();
}

bool SDLIO::run()
{
	if (!open())
		return false;
	
	bool isRunning = true;
	while (isRunning)
	{
		SDL_Event event;
        
		if (!SDL_WaitEvent(&event))
			break;
		
		switch (event.type)
		{
			case SDL_ACTIVEEVENT:
				if (event.active.gain == 0)
					releaseMouse();
				break;
			case SDL_KEYDOWN:
				pushKeyboardKey(event.key.keysym.sym,
								event.key.keysym.mod,
								event.key.keysym.unicode);
				break;
			case SDL_KEYUP:
				pullKeyboardKey(event.key.keysym.sym,
								event.key.keysym.mod,
								event.key.keysym.unicode);
				break;
			case SDL_MOUSEMOTION:
				if (isMouseCaptured())
					moveMouse(event.motion.xrel,
							  event.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (isMouseCaptured())
					pushMouseButton(event.button.button);
				else
					captureMouse();
				break;
			case SDL_MOUSEBUTTONUP:
				if (isMouseCaptured())
					pullMouseButton(event.button.button);
				break;
			case SDL_JOYAXISMOTION:
				setJoystickPosition(event.jaxis.axis, 
									(event.jaxis.value + 32768) / 256);
				break;
			case SDL_JOYBUTTONDOWN:
				pushJoystickButton(event.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				pullJoystickButton(event.jbutton.button);
				break;
			case SDL_USEREVENT:
				close();
				open();
				// motherboard->getVideoRenderer()->updateVideo();
				break;
			case SDL_QUIT:
				isRunning = false;
				break;
		}
    }
	
	close();
	
	return true;
}

bool SDLIO::sendReopenEvent()
{
	SDL_UserEvent event;
	
	event.type = SDL_USEREVENT;
	event.code = 0;
	return (SDL_PushEvent((SDL_Event *) &event) != -1);
}

bool SDLIO::sendQuitEvent()
{
	SDL_Event event;
	event.type = SDL_QUIT;
	return (SDL_PushEvent(&event) != -1);
}
