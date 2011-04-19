
/**
 * libemulation-hal
 * OpenGL canvas
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenGL canvas.
 */

#ifndef _OPENGLCANVAS_H
#define _OPENGLCANVAS_H

#include <pthread.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "OEComponent.h"
#include "CanvasInterface.h"

typedef enum
{
	OPENGLCANVAS_CAPTURE_NONE,
	OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR,
	OPENGLCANVAS_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR,
} OpenGLCanvasCapture;

typedef enum
{
	OPENGLCANVAS_TEXTURE_SHADOWMASK_TRIAD,
	OPENGLCANVAS_TEXTURE_SHADOWMASK_INLINE,
	OPENGLCANVAS_TEXTURE_SHADOWMASK_APERTURE,
	OPENGLCANVAS_TEXTURE_BEZEL_POWER,
	OPENGLCANVAS_TEXTURE_BEZEL_PAUSE,
	OPENGLCANVAS_TEXTURE_BEZEL_CAPTURE,
	OPENGLCANVAS_TEXTURE_FRAME_RAW,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED_LAST1,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED_LAST2,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED_LAST3,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED_LAST4,
	OPENGLCANVAS_TEXTURE_FRAME_PROCESSED_LAST5,
	OPENGLCANVAS_TEXTURE_END,
} OpenGLCanvasTextureIndex;

typedef enum
{
	OPENGLCANVAS_PROGRAM_RGB,
	OPENGLCANVAS_PROGRAM_NTSC,
	OPENGLCANVAS_PROGRAM_PAL,
	OPENGLCANVAS_PROGRAM_SCREEN,
	OPENGLCANVAS_PROGRAM_END,
} OpenGLCanvasProgram;



typedef void (*CanvasSetCapture)(void *userData, OpenGLCanvasCapture capture);
typedef void (*CanvasSetKeyboardFlags)(void *userData, int flags);



class OpenGLCanvas : public OEComponent
{
public:
	OpenGLCanvas(string resourcePath);
	~OpenGLCanvas();
	
	void open(CanvasSetCapture setCapture,
			  CanvasSetKeyboardFlags setKeyboardFlags,
			  void *userData);
	void close();
	
	OESize getCanvasSize();
	CanvasMode getCanvasMode();
	void setEnableGLSL(bool value);
	bool update(float width, float height, float offset, bool redraw);
	
	void becomeKeyWindow();
	void resignKeyWindow();
	void setKey(int usageId, bool value);
	void sendUnicodeKeyEvent(int unicode);
	
	void setMouseButton(int index, bool value);
	void enterMouse();
	void exitMouse();
	void setMousePosition(float x, float y);
	void moveMouse(float rx, float ry);
	void sendMouseWheelEvent(int index, float value);
	
	void setJoystickButton(int deviceIndex, int index, bool value);
	void setJoystickPosition(int deviceIndex, int index, float value);
	void sendJoystickHatEvent(int deviceIndex, int index, float value);
	void moveJoystickBall(int deviceIndex, int index, float value);
	
	void copy(string& value);
	void paste(string value);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	string resourcePath;
	
	CanvasSetCapture setCapture;
	CanvasSetKeyboardFlags setKeyboardFlags;
	void *userData;
	
	bool isOpen;
	bool isGLSL;
	
	pthread_mutex_t mutex;
	
	bool isNewConfiguration;
	CanvasConfiguration configuration;
	CanvasConfiguration drawConfiguration;
	bool isNewFrame;
	OEImage frame;
	
	OESize viewportSize;
	GLuint texture[OPENGLCANVAS_TEXTURE_END];
	OESize textureSize[OPENGLCANVAS_TEXTURE_END];
	OESize frameSize;
	GLuint program[OPENGLCANVAS_PROGRAM_END];
	GLuint processProgram;
	
	CanvasBezel bezel;
	bool isBezelUpdated;
	int captureBezelCount;
	
	OpenGLCanvasCapture capture;
	
	bool keyDown[CANVAS_KEYBOARD_KEY_NUM];
	int keyDownCount;
	bool ctrlAltWasPressed;
	bool mouseEntered;
	bool mouseButtonDown[CANVAS_MOUSE_BUTTON_NUM];
	bool joystickButtonDown[CANVAS_JOYSTICK_NUM][CANVAS_JOYSTICK_BUTTON_NUM];
	
	void lock();
	void unlock();
	
	bool initOpenGL();
	void freeOpenGL();
	GLuint getGLFormat(OEImageFormat format);
	void loadTextures();
	void loadTexture(string path, bool isMipmap, int textureIndex);
	void loadPrograms();
	void deletePrograms();
	GLuint loadProgram(const char *source);
	void deleteProgram(GLuint index);
	void updateViewport();
	void updateConfiguration();
	void setTextureSize(GLuint program);
	bool uploadFrame();
	void processFrame();
	void drawCanvas();
	
	void drawBezel();
	
	void postHIDNotification(int notification, int usageId, float value);
	void updateCapture(OpenGLCanvasCapture capture);
	void resetKeysAndButtons();
	
	bool setCaptureMode(CanvasCaptureMode *captureMode);
	bool setConfiguration(CanvasConfiguration *configuration);
	bool postFrame(OEImage *frame);
};

#endif
