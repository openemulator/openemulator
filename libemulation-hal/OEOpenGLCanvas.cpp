
/**
 * OpenEmulator
 * OpenEmulator OpenGL canvas
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator OpenGL canvas.
 */

#include "OEOpenGLCanvas.h"

typedef struct
{
	float x;
	float y;
} OEPoint;

typedef struct
{
	float width;
	float height;
} OESize;

typedef struct
{
	OEPoint origin;
	OESize size;
} OERect;

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

OEOpenGLCanvas::OEOpenGLCanvas()
{
	// Init structures
	//	pthread_mutex_init(&glMutex, NULL);
	memset(keyDown, sizeof(keyDown), 0);
	memset(mouseButtonDown, sizeof(mouseButtonDown), 0);
	memset(joystickButtonDown, sizeof(joystickButtonDown), 0);
	
	mouseCaptured = false;
}

OEOpenGLCanvas::~OEOpenGLCanvas()
{
	glDeleteTextures(OEGL_TEX_NUM, textures);
}

void OEOpenGLCanvas::initOpenGL()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	glGenTextures(OEGL_TEX_NUM, textures);
}

void OEOpenGLCanvas::draw(int width, int height)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void OEOpenGLCanvas::postHIDNotification(int notification, int usageId, float value)
{
	HostCanvasHIDNotification data;
	
	data.usageId = usageId;
	data.value = value;
	
	notify(this, notification, &data);
}

void OEOpenGLCanvas::setSystemKey(int usageId)
{
	postHIDNotification(HOST_CANVAS_SYSTEMKEYBOARD_DID_CHANGE, usageId, 0);
}

void OEOpenGLCanvas::setKey(int usageId, bool value)
{
	if (keyDown[usageId] == value)
		return;
	keyDown[usageId] = value;
	
	postHIDNotification(HOST_CANVAS_KEYBOARD_DID_CHANGE, usageId, value);
	
	if ((keyDown[HOST_CANVAS_K_LEFTCONTROL] ||
		 keyDown[HOST_CANVAS_K_RIGHTCONTROL]) &&
		(keyDown[HOST_CANVAS_K_LEFTALT] ||
		 keyDown[HOST_CANVAS_K_RIGHTALT]))
		mouseCaptureRelease = true;
	
	if (mouseCaptureRelease && !keyDownCount)
	{
		mouseCaptureRelease = false;
		mouseCaptured = false;
		//		setMouseCapture(emulation, false);
	}
}

void OEOpenGLCanvas::setUnicodeKey(int unicode)
{
	if (unicode == 127)
		unicode = 8;
	
	// Discard private usage areas
	if (((unicode < 0xe000) || (unicode > 0xf8ff)) &&
		((unicode < 0xf0000) || (unicode > 0xffffd)) &&
		((unicode < 0x100000) || (unicode > 0x10fffd)))
		postHIDNotification(HOST_CANVAS_UNICODEKEYBOARD_DID_CHANGE, unicode, 0);
}

void OEOpenGLCanvas::setMouseButton(int index, bool value)
{
	if (index >= HOST_CANVAS_MOUSE_BUTTON_NUM)
		return;
	
	if (mouseButtonDown[index] == value)
		return;
	
	mouseButtonDown[index] = value;
	
	if (mouseCaptured)
		postHIDNotification(HOST_CANVAS_MOUSE_DID_CHANGE,
							HOST_CANVAS_M_BUTTON1 + index,
							value);
	/*	else if (!mouseCaptured && mouseCapture && (index == 0))
	 {
	 mouseCaptured = true;
	 setMouseCapture(emulation, true);
	 }
	 */	else
		 postHIDNotification(HOST_CANVAS_POINTER_DID_CHANGE,
							 HOST_CANVAS_P_BUTTON1 + index,
							 value);
}

void OEOpenGLCanvas::setMousePosition(float x, float y)
{
	if (mouseCaptured)
		return;
	
	postHIDNotification(HOST_CANVAS_POINTER_DID_CHANGE,
						HOST_CANVAS_P_X,
						x);
	postHIDNotification(HOST_CANVAS_POINTER_DID_CHANGE,
						HOST_CANVAS_P_Y,
						y);
}

void OEOpenGLCanvas::moveMouse(float rx, float ry)
{
	if (!mouseCaptured)
		return;
	
	postHIDNotification(HOST_CANVAS_MOUSE_DID_CHANGE,
						HOST_CANVAS_M_RELX,
						rx);
	postHIDNotification(HOST_CANVAS_MOUSE_DID_CHANGE,
						HOST_CANVAS_M_RELY,
						ry);
}

void OEOpenGLCanvas::sendMouseWheelEvent(int index, float value)
{
	if (!value)
		return;
	
	if (mouseCaptured)
		postHIDNotification(HOST_CANVAS_MOUSE_DID_CHANGE,
							HOST_CANVAS_M_WHEELX + index,
							value);
	else
		postHIDNotification(HOST_CANVAS_POINTER_DID_CHANGE,
							HOST_CANVAS_P_WHEELX + index,
							value);
}

void OEOpenGLCanvas::setJoystickButton(int deviceIndex, int index, bool value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_BUTTON_NUM)
		return;
	
	if (joystickButtonDown[deviceIndex][index] == value)
		return;
	
	joystickButtonDown[deviceIndex][index] = value;
	
	postHIDNotification(HOST_CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						HOST_CANVAS_J_BUTTON1 + index,
						value);
}

void OEOpenGLCanvas::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_AXIS_NUM)
		return;
	
	postHIDNotification(HOST_CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						HOST_CANVAS_J_AXIS1 + index,
						value);
}

void OEOpenGLCanvas::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_HAT_NUM)
		return;
	
	postHIDNotification(HOST_CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						HOST_CANVAS_J_AXIS1 + index,
						value);
}

void OEOpenGLCanvas::moveJoystickBall(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_CANVAS_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_CANVAS_JOYSTICK_RAXIS_NUM)
		return;
	
	postHIDNotification(HOST_CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						HOST_CANVAS_J_AXIS1 + index,
						value);
}

void OEOpenGLCanvas::resetKeysAndButtons()
{
	for (int i = 0; i < sizeof(keyDown); i++)
	{
		
	}
	
}



/*
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
 
 /*void OEGL::update(HostVideoScreens *screens)
 {
 pthread_mutex_lock(&glMutex);
 
 this->screens = *screens;
 updateCellSize();
 updateScreenMatrix();
 
 windowRedraw = true;
 
 pthread_mutex_unlock(&glMutex);
 }
 
 void OEGL::draw(int width, int height)
 {
 pthread_mutex_lock(&glMutex);
 
 glViewport(0, 0, width, height);
 
 if ((width != windowSize.width) || (height != windowSize.height))
 {
 windowSize = OEMakeSize(width, height);
 windowAspectRatio = OERatio(windowSize);
 windowRedraw = true;
 }
 //	if (windowRedraw)
 glClear(GL_COLOR_BUFFER_BIT);
 int screenIndex = 0;
 for (HostVideoScreens::iterator i = screens.begin();
 i != screens.end();
 i++)
 {
 drawScreen(*i, screenIndex);
 screenIndex++;
 }
 
 windowRedraw = false;
 
 pthread_mutex_unlock(&glMutex);
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
 */