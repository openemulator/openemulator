
/**
 * OpenEmulator
 * OpenGL HAL
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenGL HAL.
 */

#include "OpenGLHAL.h"

#define DEFAULT_WIDTH	640
#define DEFAULT_HEIGHT	480

OpenGLHAL::OpenGLHAL()
{
	setCapture = NULL;
	setKeyboardFlags = NULL;
	
	configuration.size.width = DEFAULT_WIDTH;
	configuration.size.height = DEFAULT_HEIGHT;
	configuration.captureMode = CANVAS_CAPTUREMODE_NO_CAPTURE;
	
	capture = OPENGLHAL_CAPTURE_NONE;
	
	frameCurrent = NULL;
	frameNext = NULL;
	
	for (int i = 0; i < CANVAS_KEYBOARD_KEY_NUM; i++)
		keyDown[i] = false;
	keyDownCount = 0;
	ctrlAltWasPressed = false;
	mouseEntered = false;
	for (int i = 0; i < CANVAS_MOUSE_BUTTON_NUM; i++)
		mouseButtonDown[i] = false;
	for (int i = 0; i < CANVAS_JOYSTICK_NUM; i++)
		for (int j = 0; j < CANVAS_JOYSTICK_BUTTON_NUM; j++)
			joystickButtonDown[i][j] = false;
}

void OpenGLHAL::open(CanvasSetCapture setCapture,
					 CanvasSetKeyboardFlags setKeyboardFlags,
					 void *userData)
{
	this->setCapture = setCapture;
	this->setKeyboardFlags = setKeyboardFlags;
	this->userData = userData;
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	glGenTextures(OPENGLHAL_TEXTURE_END, glTextures);
	
	pthread_mutex_init(&frameMutex, NULL);
}

void OpenGLHAL::close()
{
	pthread_mutex_destroy(&frameMutex);
	
	glDeleteTextures(OPENGLHAL_TEXTURE_END, glTextures);
}

void OpenGLHAL::updateCapture(OpenGLHALCapture capture)
{
//	log("updateCapture");
	
	if (this->capture == capture)
		return;
	this->capture = capture;
	
	if (setCapture)
		setCapture(userData, capture);
}

void OpenGLHAL::postHIDNotification(int notification, int usageId, float value)
{
	CanvasHIDNotification data = {usageId, value};
	notify(this, notification, &data);
}

OESize OpenGLHAL::getDefaultSize()
{
	return configuration.size;
}

void OpenGLHAL::update(float width, float height, float offset)
{
	// Get new frame
	OEImage *framePrevious = NULL;
	bool redraw = false;
	
	pthread_mutex_lock(&frameMutex);
	if (frameNext)
	{
		framePrevious = frameCurrent;
		frameCurrent = frameNext;
		frameNext = NULL;
		redraw = true;
	}
	pthread_mutex_unlock(&frameMutex);
	
	if (framePrevious)
		delete framePrevious;
	
	if (redraw)
	{
		// Prepare OpenGL
		GLint glFormat;
		GLvoid *glPixels;
		
		if (frameCurrent->getFormat() == OEIMAGE_FORMAT_MONOCHROME)
			glFormat = GL_LUMINANCE;
		else if (frameCurrent->getFormat() == OEIMAGE_FORMAT_RGB)
			glFormat = GL_RGB;
		else if (frameCurrent->getFormat() == OEIMAGE_FORMAT_RGBA)
			glFormat = GL_RGBA;
		textureSize = frameCurrent->getSize();
		glPixels = frameCurrent->getPixels();
		
		// Upload texture
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT,
					  glTextures[OPENGLHAL_TEXTURE_FRAME]);
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
					 0, glFormat, textureSize.width, textureSize.height,
					 0, glFormat, GL_UNSIGNED_BYTE, glPixels
					 );
		
		draw(width, height, offset);
	}
}

void OpenGLHAL::draw(float width, float height, float offset)
{
	// Reset
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (!frameCurrent)
		return;
	
	// Render canvas
	OERect frame;
	
	float textureProportion = textureSize.width / textureSize.height;
	float viewProportion = width / height;
	
	if (textureProportion > viewProportion)
	{
		float ratio = viewProportion / textureProportion;
		frame.origin.x = -1.0;
		frame.origin.y = -ratio;
		frame.size.width = 2.0;
		frame.size.height = 2.0 * ratio;
	}
	else
	{
		float ratio = textureProportion / viewProportion;
		frame.origin.x = -ratio;
		frame.origin.y = -1.0;
		frame.size.width = 2.0 * ratio;
		frame.size.height = 2.0;
	}
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(OEMinX(frame), OEMaxY(frame));
	glTexCoord2f(textureSize.width, 0);
	glVertex2f(OEMaxX(frame), OEMaxY(frame));
	glTexCoord2f(textureSize.width, textureSize.height);
	glVertex2f(OEMaxX(frame), OEMinY(frame));
	glTexCoord2f(0, textureSize.height);
	glVertex2f(OEMinX(frame), OEMinY(frame));
	glEnd();
}

void OpenGLHAL::becomeKeyWindow()
{
}

void OpenGLHAL::resignKeyWindow()
{
	resetKeysAndButtons();
	
	ctrlAltWasPressed = false;
	
	updateCapture(OPENGLHAL_CAPTURE_NONE);
}

void OpenGLHAL::sendSystemEvent(int usageId)
{
	postHIDNotification(CANVAS_SYSTEMKEYBOARD_DID_CHANGE, usageId, 0);
}

void OpenGLHAL::setKey(int usageId, bool value)
{
	if (keyDown[usageId] == value)
		return;
	
//	log("key " + getHexString(usageId) + ": " + getString(value));
//	log("keyDownCount " + getString(keyDownCount));
	
	keyDown[usageId] = value;
	keyDownCount += value ? 1 : -1;
	if ((keyDown[CANVAS_K_LEFTCONTROL] ||
		 keyDown[CANVAS_K_RIGHTCONTROL]) &&
		(keyDown[CANVAS_K_LEFTALT] ||
		 keyDown[CANVAS_K_RIGHTALT]))
		ctrlAltWasPressed = true;
	
	postHIDNotification(CANVAS_KEYBOARD_DID_CHANGE, usageId, value);
	
	if ((capture == OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR) &&
		!keyDownCount && ctrlAltWasPressed)
	{
		ctrlAltWasPressed = false;
		
		updateCapture(OPENGLHAL_CAPTURE_NONE);
	}
}

void OpenGLHAL::sendUnicodeKeyEvent(int unicode)
{
//	log("unicode " + getHexString(unicode));
	
	postHIDNotification(CANVAS_UNICODEKEYBOARD_DID_CHANGE, unicode, 0);
}

void OpenGLHAL::enterMouse()
{
	mouseEntered = true;
	
	if (configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
		updateCapture(OPENGLHAL_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR);
	
	postHIDNotification(CANVAS_POINTER_DID_CHANGE,
						CANVAS_P_PROXIMITY,
						1.0);
}

void OpenGLHAL::exitMouse()
{
	mouseEntered = false;
	
	if (configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
		updateCapture(OPENGLHAL_CAPTURE_NONE);
	
	postHIDNotification(CANVAS_POINTER_DID_CHANGE,
						CANVAS_P_PROXIMITY,
						0.0);
}

void OpenGLHAL::setMousePosition(float x, float y)
{
	if (capture != OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
	{
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_X,
							x);
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_Y,
							y);
	}
}

void OpenGLHAL::moveMouse(float rx, float ry)
{
	if (capture == OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
	{
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_RELX,
							rx);
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_RELY,
							ry);
	}
}

void OpenGLHAL::setMouseButton(int index, bool value)
{
	if (index >= CANVAS_MOUSE_BUTTON_NUM)
		return;
	if (mouseButtonDown[index] == value)
		return;
	mouseButtonDown[index] = value;
	
	if (capture == OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_BUTTON1 + index,
							value);
	else if ((configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK) &&
			 (capture == OPENGLHAL_CAPTURE_NONE) &&
			 (index == 0))
		updateCapture(OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR);
	else
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_BUTTON1 + index,
							value);
}

void OpenGLHAL::sendMouseWheelEvent(int index, float value)
{
	if (capture == OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_WHEELX + index,
							value);
	else
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_WHEELX + index,
							value);
}

void OpenGLHAL::setJoystickButton(int deviceIndex, int index, bool value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_BUTTON_NUM)
		return;
	if (joystickButtonDown[deviceIndex][index] == value)
		return;
	joystickButtonDown[deviceIndex][index] = value;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_BUTTON1 + index,
						value);
}

void OpenGLHAL::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_AXIS_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLHAL::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_HAT_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLHAL::moveJoystickBall(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_RAXIS_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLHAL::resetKeysAndButtons()
{
	for (int i = 0; i < CANVAS_KEYBOARD_KEY_NUM; i++)
		setKey(i, false);
	
	for (int i = 0; i < CANVAS_MOUSE_BUTTON_NUM; i++)
		setMouseButton(i, false);
	
	for (int i = 0; i < CANVAS_JOYSTICK_NUM; i++)
		for (int j = 0; j < CANVAS_JOYSTICK_BUTTON_NUM; j++)
			setJoystickButton(i, j, false);
}

bool OpenGLHAL::copy(string &value)
{
	return OEComponent::postMessage(this, CANVAS_COPY, &value);
}

bool OpenGLHAL::paste(string value)
{
	return OEComponent::postMessage(this, CANVAS_PASTE, &value);
}

bool OpenGLHAL::setConfiguration(CanvasConfiguration *configuration)
{
	if (!configuration)
		return false;
	
	if (this->configuration.captureMode != configuration->captureMode)
	{
		switch (configuration->captureMode)
		{
			case CANVAS_CAPTUREMODE_NO_CAPTURE:
				updateCapture(OPENGLHAL_CAPTURE_NONE);
				break;
				
			case CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK:
				updateCapture(OPENGLHAL_CAPTURE_NONE);
				break;
				
			case CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER:
				updateCapture(mouseEntered ? 
							  OPENGLHAL_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR : 
							  OPENGLHAL_CAPTURE_NONE);
				break;
		}
	}
	
	this->configuration = *configuration;
	
	return true;
}

bool OpenGLHAL::requestFrame(OEImage **frame)
{
	if (!frame)
		return false;
	
	*frame = new OEImage();
	
	return (*frame != NULL);
}

bool OpenGLHAL::returnFrame(OEImage **frame)
{
	if (!frame)
		return false;
	
	// Post frame
	OEImage *frameDiscarded = NULL;
	
	pthread_mutex_lock(&frameMutex);
	if (frameNext)
		frameDiscarded = frameNext;
	frameNext = *frame;
	pthread_mutex_unlock(&frameMutex);
	
	if (frameDiscarded)
		delete frameDiscarded;
	
	*frame = NULL;
	
	return true;
}

bool OpenGLHAL::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case CANVAS_CONFIGURE:
			return setConfiguration((CanvasConfiguration *)data);
			
		case CANVAS_REQUEST_FRAME:
			return requestFrame((OEImage **)data);
			
		case CANVAS_RETURN_FRAME:
			return returnFrame((OEImage **)data);
			
		case CANVAS_LOCK_OPENGL:
			break;
			
		case CANVAS_UNLOCK_OPENGL:
			break;
	}
	
	return OEComponent::postMessage(sender, message, data);
}



/*
 
 Shader goes here:
 
 *
 * GLSL NTSC Shader 1.01
 *
 * (C) 2010 by Marc S. Ressl/Gortu
 * Takes as input a raw NTSC frame
 *
 * Parameters:
 * size_*          - texture size
 * comp_fsc        - subcarrier to sampling ratio
 *                   (Apple II uses 0.25)
 * comp_black      - composite black level
 *                   (usually 0.2)
 * comp_white      - composite white level
 *                   (usually 0.8)
 * comp_hue        - hue phase adjustment
 *                   (0 to 1)
 * comp_saturation - chroma gain
 *                   (0 to 1)
 * rgb_*           - rgb gains
 *                   (0 to 1, 1 default)
 * vid_barrel      - Monitor barrel effect
 *                   (0 to 1)
 * vid_brightness  - Monitor brightness
 *                   (-1 to 1, 0 default)
 * vid_contrast    - Monitor contrast
 *                   (0 to 1, 1 default)
 *
 
 uniform sampler2DRect texture;
 uniform float size_x;
 uniform float size_y;
 uniform float comp_fsc;
 uniform float comp_black;
 uniform float comp_white;
 uniform float comp_saturation;
 uniform float comp_hue;
 uniform float rgb_red;
 uniform float rgb_green;
 uniform float rgb_blue;
 uniform float vid_contrast;
 uniform float vid_brightness;
 uniform float vid_barrel;
 
 vec3 demodulate(vec2 q, float i)
 {
 float phase1 = 2.0 * 3.1415926535 * comp_fsc * (q.x + i);
 vec3 p = texture2DRect(texture, vec2(q.x + i, q.y)).xyz;
 p -= comp_black;
 p.y *= sin(phase1);
 p.z *= cos(phase1);
 return p;
 }
 
 vec3 filter(vec2 q, float i, float cy, float cc)
 {
 return (demodulate(q, i) + demodulate(q, -i)) * vec3(cy, cc, cc);
 }
 
 // x, y, z is used as Y'UV
 void main(void)
 {
 float hue = 2.0 * 3.1415926535 * comp_hue;
 
 // Decoder matrix
 mat3 decoderMatrix = mat3(
 1.0, 0.0, 0.0,
 0.0, 1.0, 0.0,
 0.0, 0.0, 1.0);
 // Contrast gain
 decoderMatrix *= vid_contrast;
 // RGB gain
 decoderMatrix *= mat3(
 rgb_red, 0.0, 0.0,
 0.0, rgb_green, 0.0,
 0.0, 0.0, rgb_blue);
 // Y'UV to RGB decoder matrix
 decoderMatrix *= mat3(
 1.0, 1.0, 1.0,
 0.0, -0.394642, 2.032062,
 1.139883,-0.580622, 0.0);
 // Hue gain
 decoderMatrix *= mat3(
 1.0, 0.0, 0.0,
 0.0, cos(hue), -sin(hue),
 0.0, sin(hue), cos(hue));
 // Saturation gain
 decoderMatrix *= mat3(
 1.0, 0.0, 0.0,
 0.0, comp_saturation, 0.0,
 0.0, 0.0, comp_saturation);
 // Demodulator gain
 decoderMatrix *= mat3(
 1.0, 0.0, 0.0,
 0.0, sqrt(2.0), 0.0,
 0.0, 0.0, sqrt(2.0));
 // Dynamic range gain
 decoderMatrix /= (comp_white - comp_black);
 
 // Barrel effect
 vec2 q = gl_TexCoord[0].xy;
 q = (q / vec2(size_x, size_y)) - vec2(0.5, 0.5);
 q += vid_barrel * q * (q.x * q.x + q.y * q.y);
 q = (q + vec2(0.5, 0.5)) * vec2(size_x, size_y);
 
 // FIR filter
 vec3 col = filter(q, 8.0, 0.001834, 0.005608678704129);
 col += filter(q, 7.0, 0.001595, 0.013136133246966);
 col += filter(q, 6.0, -0.006908, 0.025634921259280);
 col += filter(q, 5.0, -0.023328, 0.042403293307818);
 col += filter(q, 4.0, -0.025079, 0.062044634750949);
 col += filter(q, 3.0, 0.023269, 0.082124466712280);
 col += filter(q, 2.0, 0.130295, 0.099609418890150);
 col += filter(q, 1.0, 0.248233, 0.111545537317893);
 col += filter(q, 0.0, 0.300177 / 2.0, 0.115785831621067 / 2.0);
 col = decoderMatrix * col + vid_brightness;
 
 gl_FragColor = vec4(col, 1.0);
 }
 */

