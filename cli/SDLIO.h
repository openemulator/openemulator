
/**
 * AppleIIGo
 * SDL IO Class
 * (C) 2007-2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef SDLIO_H
#define SDLIO_H

#include <string>
#include <list>
#include <map>

#include <stdint.h>

#include <SDL.h>

#include "ComponentBoard.h"

#define FRAMEBUFFER_X	640
#define FRAMEBUFFER_Y	480
#define MOUSEBUTTON_NUM 3

#define SDLIO_VIDEO_X			FRAMEBUFFER_X
#define SDLIO_VIDEO_Y			FRAMEBUFFER_Y

#define SDLIO_AUDIO_SAMPLENUM			8192
#define SDLIO_AUDIO_SAMPLERATE			48000
#define SDLIO_AUDIO_VOLUMEFILTERFACTOR	(400.0 / SDLIO_AUDIO_SAMPLERATE)

#define saturateAudio(value) (((value) > 32767) ? 32767 : ((value) < -32768) \
	? 32768 : (value))

class SDLIO {
public:
	SDLIO(map<string, string> &parameters);
	~SDLIO();
	
	void sendSdlIoMessage(uint32_t messageIndex, void * messageData);
	void sendComponentMessage(string &destination, void * messageData);
	
	static int runSdlThread(void * userdata);
	void lock();
	void unlock();
	void setPause(bool value);
	bool isPaused();
	void runThread();
	
	static void callbackSdl(void * userdata, uint8_t * stream, int len);
	void callbackAudio(int16_t * sampleData, uint32_t sampleNum);
	
	void setFullscreen(bool value);
	void toggleFullscreen();
	bool isFullscreen();
	
	bool run();
	bool sendQuitEvent();
	bool sendReopenEvent();
	
private:
	ComponentBoard * componentBoard;
	
	SDL_mutex * mutex;
	SDL_cond * cond;
	SDL_Thread * thread;
	bool isThreadPausedValue;
	bool isKillThreadSignal;
	
	bool isPalTiming;
	int32_t * audioInputBuffer;
	int32_t * audioOutputBuffer;
	uint32_t audioBufferSampleNum;
	uint32_t audioFrameSampleNum;
//	float audioVolume;
//	float currentAudioVolume;
	
	bool isFullscreenValue;
	SDL_Surface * videoSurface;
	const char * videoCaption;
	
	list<uint32_t> keyDownList;
	uint32_t resetCount;
	
	bool isMouseCapturedValue;
	bool isFirstMouseMovementSkipped;
	bool isMouseButtonDown[MOUSEBUTTON_NUM];
	
	SDL_Joystick * joystick;
	
	bool open();
	void close();
	
	void initThread();
	bool openThread();
	void closeThread();
	
	void initAudio();
	bool openAudio();
	void closeAudio();
	
	void initVideo();
	bool openVideo(const char * caption);
	void closeVideo();
	
	void initKeyboard();
	void openKeyboard();
	void closeKeyboard();
	void pushKeyboardKey(uint32_t sym, uint32_t mod, uint32_t unicode);
	void pullKeyboardKey(uint32_t sym, uint32_t mod, uint32_t unicode);
	uint32_t getKeyboardModifierIndex(uint32_t sym);
	
	void initMouse();
	void openMouse();
	void closeMouse();
	void moveMouse(int32_t xRel, int32_t yRel);
	void pushMouseButton(uint32_t button);
	void pullMouseButton(uint32_t button);
	void clearMouseButtons();
	void captureMouse();
	void releaseMouse();
	bool isMouseCaptured();
	uint32_t getMouseButtonIndex(uint32_t button);
	
	void initJoystick();
	void openJoystick();
	void closeJoystick();
	void setJoystickPosition(uint32_t axis, uint32_t value);
	void pushJoystickButton(uint32_t button);
	void pullJoystickButton(uint32_t button);
};

#endif
