
/**
 * OpenEmulator
 * OpenEmulator OpenGL canvas interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator OpenGL canvas interface.
 */

#include "OEOpenGLCanvas.h"

OEHID::OEHID(OEPAEmulation *emulation,
			 OEHIDCallback setMouseCapture,
			 OEHIDCallback setKeyboardLEDs)
{
	this->emulation = emulation;
	this->setMouseCapture = setMouseCapture;
	this->setKeyboardLEDs = setKeyboardLEDs;
	
	keyDownCount = 0;
	memset(keyDown, sizeof(keyDown), 0);
	memset(mouseButtonDown, sizeof(mouseButtonDown), 0);
	memset(joystickButtonDown, sizeof(joystickButtonDown), 0);
	
	string value;
	//	emulation->getProperty(HOST_DEVICE, "hidMouseCapture", value);
	mouseCapture = (value == "1");
	mouseCaptured = false;
}

void OEHID::sendHIDEvent(int notification, int usageId, float value)
{
	HostCanvasNotification notificationInfo;
	notificationInfo.usageId = usageId;
	notificationInfo.value = value;
	
	if (emulation)
		emulation->notify("", notification, &notificationInfo);
	
	printf("HID event: %d %d %f\n", notification, usageId, value);
}

void OEHID::sendSystemEvent(int usageId)
{
	sendHIDEvent(HOST_CANVAS_SYSTEMKEYBOARD_CHANGED, usageId, 0);
}

void OEHID::setKey(int usageId, bool value)
{
	if (keyDown[usageId] == value)
		return;
	
	keyDown[usageId] = value;
	
	sendHIDEvent(HOST_CANVAS_KEYBOARD_CHANGED, usageId, value);
	
	int count = keyDownCount + (value ? 1 : -1);
	keyDownCount = count < 0 ? 0 : count;
	
	if ((keyDown[HOST_CANVAS_K_LEFTCONTROL] ||
		 keyDown[HOST_CANVAS_K_RIGHTCONTROL]) &&
		(keyDown[HOST_CANVAS_K_LEFTALT] ||
		 keyDown[HOST_CANVAS_K_RIGHTALT]))
		mouseCaptureRelease = true;
	
	if (mouseCaptureRelease && !keyDownCount)
	{
		mouseCaptureRelease = false;
		mouseCaptured = false;
		setMouseCapture(emulation, false);
	}
}

void OEHID::sendUnicode(int unicode)
{
	if (unicode == 127)
		unicode = 8;
	
	// Discard private usage areas
	if (((unicode < 0xe000) || (unicode > 0xf8ff)) &&
		((unicode < 0xf0000) || (unicode > 0xffffd)) &&
		((unicode < 0x100000) || (unicode > 0x10fffd)))
		sendHIDEvent(HOST_CANVAS_UNICODEKEYBOARD_CHANGED, unicode, 0);
}

void OEHID::setMouseButton(int index, bool value)
{
	if (index >= HOST_CANVAS_MOUSE_BUTTON_NUM)
		return;
	
	if (mouseButtonDown[index] == value)
		return;
	
	mouseButtonDown[index] = value;
	
	if (mouseCaptured)
		sendHIDEvent(HOST_CANVAS_MOUSE_CHANGED,
					 HOST_CANVAS_M_BUTTON1 + index,
					 value);
	else if (!mouseCaptured && mouseCapture && (index == 0))
	{
		mouseCaptured = true;
		setMouseCapture(emulation, true);
	}
	else
		sendHIDEvent(HOST_CANVAS_POINTER_CHANGED,
					 HOST_CANVAS_P_BUTTON1 + index,
					 value);
}

void OEHID::setMousePosition(float x, float y)
{
	if (mouseCaptured)
		return;
	
	sendHIDEvent(HOST_CANVAS_POINTER_CHANGED,
				 HOST_CANVAS_P_X,
				 x);
	sendHIDEvent(HOST_CANVAS_POINTER_CHANGED,
				 HOST_CANVAS_P_Y,
				 y);
}

void OEHID::moveMouse(float rx, float ry)
{
	if (!mouseCaptured)
		return;
	
	sendHIDEvent(HOST_CANVAS_MOUSE_CHANGED,
				 HOST_CANVAS_M_RX,
				 rx);
	sendHIDEvent(HOST_CANVAS_MOUSE_CHANGED,
				 HOST_CANVAS_M_RY,
				 ry);
}

void OEHID::sendMouseWheelEvent(int index, float value)
{
	if (!value)
		return;
	
	if (mouseCaptured)
		sendHIDEvent(HOST_CANVAS_MOUSE_CHANGED,
					 HOST_CANVAS_M_WX + index,
					 value);
	else
		sendHIDEvent(HOST_CANVAS_POINTER_CHANGED,
					 HOST_CANVAS_P_WX + index,
					 value);
}

void OEHID::setJoystickButton(int deviceIndex, int index, bool value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_BUTTON_NUM)
		return;
	
	if (joystickButtonDown[deviceIndex][index] == value)
		return;
	
	joystickButtonDown[deviceIndex][index] = value;
	
	sendHIDEvent(HOST_CANVAS_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_CANVAS_J_BUTTON1 + index,
				 value);
}

void OEHID::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_AXIS_NUM)
		return;
	
	sendHIDEvent(HOST_CANVAS_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_CANVAS_J_AXIS1 + index,
				 value);
}

void OEHID::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_HAT_NUM)
		return;
	
	sendHIDEvent(HOST_CANVAS_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_CANVAS_J_AXIS1 + index,
				 value);
}

void OEHID::moveJoystickBall(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_RAXIS_NUM)
		return;
	
	sendHIDEvent(HOST_CANVAS_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_CANVAS_J_AXIS1 + index,
				 value);
}


inline OEPoint OEMakePoint(float x, float y)
{
	OEPoint p;
	p.x = x;
	p.y = y;
	return p;
}

inline OESize OEMakeSize(float w, float h)
{
	OESize s;
	s.width = w;
	s.height = h;
	return s;
}

inline OERect OEMakeRect(float x, float y, float w, float h)
{
	OERect r;
	r.origin.x = x;
	r.origin.y = y;
	r.size.width = w;
	r.size.height = h;
	return r;
}

inline float OERatio(OESize s)
{
	return s.width ? (s.width / s.height) : 1.0F;
}

inline float OEMinX(OERect r)
{
	return r.origin.x;
}

inline float OEMaxX(OERect r)
{
	return r.origin.x + r.size.width;
}

inline float OEMinY(OERect r)
{
	return r.origin.y;
}

inline float OEMaxY(OERect r)
{
	return r.origin.y + r.size.height;
}

OEGL::OEGL()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0, 0.0, 0.0, 0.5);
	
	glGenTextures(OEGL_TEX_NUM, texture);
	
	pthread_mutex_init(&glMutex, NULL);
	
	windowSize = OEMakeSize(0, 0);
	
	columnNum = 1;
	rowNum = 1;
	cellSize = OEMakeSize(0, 0);
}

OEGL::~OEGL()
{
	glDeleteTextures(OEGL_TEX_NUM, texture);
	
	pthread_mutex_destroy(&glMutex);
}

/*void OEGL::update(HostVideoScreens *screens)
 {
 pthread_mutex_lock(&glMutex);
 
 this->screens = *screens;
 updateCellSize();
 updateScreenMatrix();
 
 windowRedraw = true;
 
 pthread_mutex_unlock(&glMutex);
 }
 */
void OEGL::draw(int width, int height)
{
	/*	pthread_mutex_lock(&glMutex);
	 
	 glViewport(0, 0, width, height);
	 
	 if ((width != windowSize.width) || (height != windowSize.height))
	 {
	 windowSize = OEMakeSize(width, height);
	 windowAspectRatio = OERatio(windowSize);
	 windowRedraw = true;
	 }
	 */	
	//	if (windowRedraw)
	glClear(GL_COLOR_BUFFER_BIT);
	/*
	 int screenIndex = 0;
	 for (HostVideoScreens::iterator i = screens.begin();
	 i != screens.end();
	 i++)
	 {
	 drawScreen(*i, screenIndex);
	 screenIndex++;
	 }
	 
	 windowRedraw = false;
	 
	 pthread_mutex_unlock(&glMutex);*/
}

OESize OEGL::getScreenSize(HostVideoConfiguration *conf)
{
	return OEMakeSize(conf->paddingLeft +	conf->contentWidth +
					  conf->paddingRight,
					  conf->paddingTop + conf->contentHeight +
					  conf->paddingBottom);
}

void OEGL::updateCellSize()
{
	cellSize = OEMakeSize(0, 0);
	
	for (HostVideoScreens::iterator i = screens.begin();
		 i != screens.end();
		 i++)
	{
		OESize size = getScreenSize(&(*i)->conf);
		
		if (size.width > cellSize.width)
			cellSize.width = size.width;
		if (size.height > cellSize.height)
			cellSize.height = size.height;
	}
}

void OEGL::updateScreenMatrix()
{
	int screenNum = screens.size();
	int value;
	
	value = (int) ceil(sqrt(screenNum));
	columnNum = value ? value : 1;
	value = (int) ceil(screenNum / columnNum);
	rowNum = value ? value : 1;
}

void OEGL::drawScreen(HostVideoScreen *screen, int index)
{
	HostVideoConfiguration *conf = &(screen->conf);
	
	// Calculate OpenGL frame coords
	int x = index % columnNum;
	int y = index / columnNum;
	
	OERect frame = OEMakeRect(2.0F * x / columnNum - 1.0F,
							  2.0F * y / rowNum - 1.0F,
							  2.0F / columnNum,
							  2.0F / rowNum);
	
	// Correct aspect ratio
	OESize screenSize = getScreenSize(conf);
	float screenAspectRatio = OERatio(screenSize);
	
	if (screenAspectRatio < windowAspectRatio)
	{
		float ratio = screenAspectRatio / windowAspectRatio;
		
		frame.origin.x += (1.0F - ratio) * frame.size.width / 2.0F;
		frame.size.width *= ratio;
	}
	else
	{
		float ratio = windowAspectRatio / screenAspectRatio;
		
		frame.origin.y += (1.0F - ratio) * frame.size.height / 2.0F;
		frame.size.height *= ratio;
	}
	
	// Correct padding
	frame.origin.x += conf->paddingLeft / screenSize.width * frame.size.width;
	frame.origin.y += conf->paddingTop / screenSize.height * frame.size.height;
	frame.size.width *= conf->contentWidth / screenSize.width;
	frame.size.height *= conf->contentHeight / screenSize.height;
	
	// Draw
	OESize framebufferSize = OEMakeSize(conf->framebufferWidth,
										conf->framebufferHeight);
	OEUInt32 *framebuffer = screen->framebuffer[0];	
	
	renderScreen(framebuffer, framebufferSize, frame);
}

void OEGL::renderScreen(OEUInt32 *framebuffer, OESize framebufferSize, OERect frame)
{
	// Upload texture
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[OEGL_TEX_FRAME]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT,
				 0, GL_RGB, framebufferSize.width, framebufferSize.height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
	
	// Render quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(OEMinX(frame), OEMaxY(frame));
	glTexCoord2f(framebufferSize.width, 0);
	glVertex2f(OEMaxX(frame), OEMaxY(frame));
	glTexCoord2f(framebufferSize.width, framebufferSize.height);
	glVertex2f(OEMaxX(frame), OEMinY(frame));
	glTexCoord2f(0, framebufferSize.height);
	glVertex2f(OEMinX(frame), OEMinY(frame));
	glEnd();
}
