
/**
 * libemulation-hal
 * OpenGL canvas
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenGL canvas
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
    OPENGLCANVAS_SHADOWMASK_TRIAD,
    OPENGLCANVAS_SHADOWMASK_INLINE,
    OPENGLCANVAS_SHADOWMASK_APERTURE,
    OPENGLCANVAS_SHADOWMASK_LCD,
    OPENGLCANVAS_SHADOWMASK_BAYER,
    OPENGLCANVAS_BEZEL_POWER,
    OPENGLCANVAS_BEZEL_PAUSE,
    OPENGLCANVAS_BEZEL_CAPTURE,
    OPENGLCANVAS_IMAGE_PHASEINFO,
    OPENGLCANVAS_IMAGE_IN,
    OPENGLCANVAS_IMAGE_DECODED,
    OPENGLCANVAS_IMAGE_PERSISTENCE,
    OPENGLCANVAS_TEXTUREEND,
} OpenGLCanvasTextureIndex;

typedef enum
{
    OPENGLCANVAS_RGB,
    OPENGLCANVAS_COMPOSITE,
    OPENGLCANVAS_DISPLAY,
    OPENGLCANVAS_SHADEREND,
} OpenGLCanvasProgram;

typedef void (*CanvasSetCapture)(void *userData, OpenGLCanvasCapture capture);
typedef void (*CanvasSetKeyboardFlags)(void *userData, OEUInt32 flags);



class OpenGLCanvas : public OEComponent
{
public:
    OpenGLCanvas(string resourcePath);
    ~OpenGLCanvas();
    
    void open(CanvasSetCapture setCapture,
              CanvasSetKeyboardFlags setKeyboardFlags,
              void *userData);
    void close();
    
    void setEnableShader(bool value);
    
    CanvasMode getMode();
    
    OESize getDefaultViewportSize();
    void setViewportSize(OESize size);
    
    OESize getSize();
    OERect getClipRect();
    void scrollPoint(OEPoint aPoint);
    OESize getPixelDensity();
    OESize getPageSize();
    OEImage getImage(OERect rect);
    
    bool vsync();
    void draw();
    
    void becomeKeyWindow();
    void resignKeyWindow();
    
    void sendUnicodeChar(CanvasUnicodeChar unicodeChar);
    
    void setKey(int usageId, bool value);
    
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
    
    void doCopy(wstring& value);
    void doPaste(wstring value);
    void doDelete();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    void postNotification(OEComponent *sender, int notification, void *data);
    bool addObserver(OEComponent *observer, int notification);
    bool removeObserver(OEComponent *observer, int notification);
    
private:
    string resourcePath;
    
    CanvasSetCapture setCapture;
    CanvasSetKeyboardFlags setKeyboardFlags;
    void *userData;
    
    bool isOpen;
    bool isShaderEnabled;
    
    pthread_mutex_t mutex;
    
    CanvasMode mode;
    CanvasCaptureMode captureMode;
    
    OESize viewportSize;
    bool isViewportUpdated;
    OEPoint clipOrigin;
    
    bool isImageUpdated;
    OEImage image;
    float imageSampleRate;
    float imageBlackLevel;
    float imageWhiteLevel;
    float imageSubcarrier;
    
    bool isConfigurationUpdated;
    
    GLuint texture[OPENGLCANVAS_TEXTUREEND];
    OESize textureSize[OPENGLCANVAS_TEXTUREEND];
    
    CanvasDisplayConfiguration displayConfiguration;
    GLuint shader[OPENGLCANVAS_SHADEREND];
    
    CanvasPaperConfiguration paperConfiguration;
    OEPoint printHead;
    
    CanvasOpenGLConfiguration openGLConfiguration;
    
    CanvasBezel bezel;
    bool isBezelDrawRequired;
    bool isBezelCapture;
    double bezelCaptureTime;
    
    OERect persistenceTexRect;
    
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
    void updateTextureSize(int textureIndex, OESize size);
    void loadShaders();
    void deleteShaders();
    void loadShader(GLuint shaderIndex, const char *source);
    void deleteShader(GLuint shaderIndex);
    
    bool uploadImage();
    GLuint getRenderShader();
    void configureShaders();
    void renderImage();
    OEPoint getDisplayCanvasTexPoint(OEPoint p);
    void drawDisplayCanvas();
    
    void drawPaperCanvas();
    
    double getCurrentTime();
    void drawBezel();
    
    OEImage readFramebuffer();
    
    void postHIDNotification(int notification, int usageId, float value);
    void updateCapture(OpenGLCanvasCapture capture);
    void resetKeysAndButtons();
    
    bool setMode(CanvasMode *mode);
    bool setCaptureMode(CanvasCaptureMode *captureMode);
    bool setBezel(CanvasBezel *bezel);
    bool setDisplayConfiguration(CanvasDisplayConfiguration *configuration);
    bool setPaperConfiguration(CanvasPaperConfiguration *configuration);
    bool setOpenGLConfiguration(CanvasOpenGLConfiguration *configuration);
    bool postImage(OEImage *frame);
    bool clear();
    bool setPrintHead(OEPoint *point);
};

#endif
