
/**
 * libemulation-hal
 * OpenGL HAL
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenGL HAL.
 */

#include <math.h>
#include <sys/time.h>

#include "OpenGLCanvas.h"

#include "OEVector.h"
#include "OEMatrix3.h"

#define NTSC_CARRIER		(0.25 * 14.31818)
#define NTSC_YIQ_CUTOFF		1.3
#define NTSC_YUV_CUTOFF		0.6
#define NTSC_YIQ_I_SHIFT	((NTSC_YUV_CUTOFF - NTSC_YIQ_CUTOFF) / NTSC_CARRIER)

#define PAPER_SLICE			256

#define BEZELCAPTURE_START	2.0
#define BEZELCAPTURE_FADEOUT 0.5

OpenGLCanvas::OpenGLCanvas(string resourcePath)
{
	this->resourcePath = resourcePath;
	
	setCapture = NULL;
	setKeyboardFlags = NULL;
	userData = NULL;
	
	isOpen = false;
	isShaderEnabled = true;
	
	pthread_mutex_init(&mutex, NULL);
	
	mode = CANVAS_MODE_DISPLAY;
	captureMode = CANVAS_CAPTUREMODE_NO_CAPTURE;
	
	viewportSize = OEMakeSize(640, 480);
	isViewportUpdated = true;
	isImageUpdated = false;
	
	printHead = OEMakePoint(0, 0);
	
	bezel = CANVAS_BEZEL_NONE;
	isBezelDrawRequired = false;
	isBezelCapture = false;
}

OpenGLCanvas::~OpenGLCanvas()
{
	pthread_mutex_destroy(&mutex);
}

void OpenGLCanvas::lock()
{
	pthread_mutex_lock(&mutex);
}

void OpenGLCanvas::unlock()
{
	pthread_mutex_unlock(&mutex);
}

// Video

void OpenGLCanvas::open(CanvasSetCapture setCapture,
					 CanvasSetKeyboardFlags setKeyboardFlags,
					 void *userData)
{
	this->setCapture = setCapture;
	this->setKeyboardFlags = setKeyboardFlags;
	this->userData = userData;
	
	initOpenGL();
}

void OpenGLCanvas::close()
{
	freeOpenGL();
}

void OpenGLCanvas::setEnableShader(bool value)
{
	lock();
	
	isShaderEnabled = value;
	isConfigurationUpdated = true;
	
	for (int i = 0; i < OPENGLCANVAS_PERSISTANCE_IMAGE_NUM; i++)
		persistance[i] = -1;
	
	unlock();
}

CanvasMode OpenGLCanvas::getMode()
{
	CanvasMode theMode;
	
	lock();
	
	theMode = mode;
	
	unlock();
	
	return theMode;
}

OESize OpenGLCanvas::getDefaultViewportSize()
{
	OESize size;
	
	lock();
	
	if (mode == CANVAS_MODE_DISPLAY)
		size = displayConfiguration.displayResolution;
	else if (mode == CANVAS_MODE_PAPER)
		size = OEMakeSize(512, 384);
	else if (mode == CANVAS_MODE_OPENGL)
		size = openGLConfiguration.viewportDefaultSize;
	
	unlock();
	
	return size;
}

void OpenGLCanvas::setViewportSize(OESize size)
{
	lock();
	
	viewportSize = size;
	isViewportUpdated = true;
	
	unlock();
}

OESize OpenGLCanvas::getSize()
{
	OESize size;
	
	lock();
	
	if (mode == CANVAS_MODE_DISPLAY)
		size = displayConfiguration.displayResolution;
	else if (mode == CANVAS_MODE_PAPER)
		size = image.getSize();
	else if (mode == CANVAS_MODE_OPENGL)
		size = viewportSize;
	
	unlock();
	
	return size;
}

OERect OpenGLCanvas::getClipRect()
{
	OERect rect;
	
	lock();
	
	if (mode == CANVAS_MODE_DISPLAY)
	{
		rect.origin = OEMakePoint(0, 0);
		rect.size = displayConfiguration.displayResolution;
	}
	else if (mode == CANVAS_MODE_PAPER)
	{
		float pixelDensityRatio = (paperConfiguration.pagePixelDensity.width /
								   paperConfiguration.pagePixelDensity.height);
		float clipHeight = (viewportSize.height * image.getSize().width /
							viewportSize.width / pixelDensityRatio);
		rect.origin = clipOrigin;
		rect.size = OEMakeSize(image.getSize().width,
							   clipHeight);
	}
	else if (mode == CANVAS_MODE_OPENGL)
	{
		rect.origin = OEMakePoint(0, 0);
		rect.size = viewportSize;
	}
	
	unlock();
	
	return rect;
}

void OpenGLCanvas::scrollPoint(OEPoint aPoint)
{
	lock();
	
	clipOrigin = aPoint;
	
	unlock();
}

OESize OpenGLCanvas::getPixelDensity()
{
	OESize pixelDensity;
	
	lock();
	
	if (mode == CANVAS_MODE_DISPLAY)
		pixelDensity = OEMakeSize(displayConfiguration.displayPixelDensity,
								  displayConfiguration.displayPixelDensity);
	else if (mode == CANVAS_MODE_PAPER)
		pixelDensity = paperConfiguration.pagePixelDensity;
	else if (mode == CANVAS_MODE_OPENGL)
		pixelDensity = OEMakeSize(openGLConfiguration.viewportPixelDensity,
								  openGLConfiguration.viewportPixelDensity);
	
	unlock();
	
	return pixelDensity;
}

OESize OpenGLCanvas::getPageSize()
{
	OESize size;
	
	lock();
	
	if (mode == CANVAS_MODE_DISPLAY)
		size = displayConfiguration.displayResolution;
	else if (mode == CANVAS_MODE_PAPER)
		size = paperConfiguration.pageResolution;
	else if (mode == CANVAS_MODE_OPENGL)
		size = viewportSize;
	
	unlock();
	
	return size;
}

OEImage OpenGLCanvas::getImage(OERect rect)
{
	if (mode == CANVAS_MODE_PAPER)
	{
		OESize size = getPageSize();
		
		return image.getClip(rect);
	}
	else 
	{
		OEImage image = readFramebuffer();
		
		OESize scale = OEMakeSize(image.getSize().height / getSize().height,
								  image.getSize().height / getSize().height);
		rect.origin.x *= scale.width;
		rect.origin.y *= scale.height;
		rect.size.width *= scale.width;
		rect.size.height *= scale.height;
		
		return image.getClip(rect);
	}
}

bool OpenGLCanvas::vsync()
{
	lock();
	
	if (isViewportUpdated)
	{
		isViewportUpdated = false;
		glViewport(0, 0, viewportSize.width, viewportSize.height);
	}
	
	CanvasVSync vSync;
	vSync.viewportSize = viewportSize;
	vSync.shouldDraw = false;
	
	if (mode == CANVAS_MODE_DISPLAY)
	{
		if (isImageUpdated)
			uploadImage();
		
		if (isConfigurationUpdated)
			updateDisplayConfiguration();
		
		if (isImageUpdated || isConfigurationUpdated)
			renderImage();
		
		updatePersistance();
		
		if (isPersistanceDrawRequired())
			vSync.shouldDraw = true;
	}
	else if (mode == CANVAS_MODE_PAPER)
		vSync.shouldDraw = isImageUpdated || isConfigurationUpdated;
	
	isImageUpdated = false;
	isConfigurationUpdated = false;
	
	if (isBezelDrawRequired)
		vSync.shouldDraw = true;
	
	unlock();
	
	notify(this, CANVAS_DID_VSYNC, &vSync);
	
	if (vSync.shouldDraw)
		draw();
	
	return vSync.shouldDraw;
}

void OpenGLCanvas::draw()
{
	if (isViewportUpdated)
	{
		lock();
		
		isViewportUpdated = false;
		glViewport(0, 0, viewportSize.width, viewportSize.height);
		
		unlock();
	}
	
	if (mode == CANVAS_MODE_DISPLAY)
	{
		lock();
		
		drawDisplayCanvas();
		
		unlock();
	}
	else if (mode == CANVAS_MODE_PAPER)
	{
		lock();
		
		drawPaperCanvas();
		
		unlock();
	}
	else
		notify(this, CANVAS_WILL_DRAW, &viewportSize);
	
	lock();
	
	drawBezel();
	
	unlock();
}

// OpenGL

bool OpenGLCanvas::initOpenGL()
{
	for (int i = 0; i < OPENGLCANVAS_TEXTURE_END; i++)
	{
		texture[i] = 0;
		textureSize[i] = OEMakeSize(0, 0);
	}
	
	isConfigurationUpdated = true;
	isShaderActive = false;
	for (int i = 0; i < OPENGLCANVAS_SHADER_END; i++)
		shader[i] = 0;
	renderIndex = 0;
	renderShader = 0;
	for (int i = 0; i < OPENGLCANVAS_PERSISTANCE_IMAGE_NUM; i++)
		persistance[i] = -1;
	
	capture = OPENGLCANVAS_CAPTURE_NONE;
	
	memset(keyDown, 0, sizeof(keyDown));
	keyDownCount = 0;
	ctrlAltWasPressed = false;
	mouseEntered = false;
	memset(mouseButtonDown, 0, sizeof(mouseButtonDown));
	memset(joystickButtonDown, 0, sizeof(joystickButtonDown));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(OPENGLCANVAS_TEXTURE_END, texture);
	loadTextures();

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	loadShaders();
	
	isOpen = true;
	
	return true;
}

void OpenGLCanvas::freeOpenGL()
{
	glDeleteTextures(OPENGLCANVAS_TEXTURE_END, texture);
	
	deleteShaders();
	
	isOpen = false;
}

GLuint OpenGLCanvas::getGLFormat(OEImageFormat format)
{
	if (format == OEIMAGE_LUMINANCE)
		return GL_LUMINANCE;
	else if (format == OEIMAGE_RGB)
		return GL_RGB;
	else if (format == OEIMAGE_RGBA)
		return GL_RGBA;
	
	return 0;
}

void OpenGLCanvas::loadTextures()
{
	loadTexture("images/Host/Shadow Mask Triad.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_TRIAD);
	loadTexture("images/Host/Shadow Mask Inline.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_INLINE);
	loadTexture("images/Host/Shadow Mask Aperture.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_APERTURE);
	loadTexture("images/Host/Shadow Mask LCD.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_LCD);
	loadTexture("images/Host/Shadow Mask Bayer.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_BAYER);
	loadTexture("images/Host/Bezel Power.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_POWER);
	loadTexture("images/Host/Bezel Pause.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_PAUSE);
	loadTexture("images/Host/Bezel Capture.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_CAPTURE);
}

void OpenGLCanvas::loadTexture(string path, bool isMipmap, int textureIndex)
{
	OEImage image;
	image.readFile(resourcePath + "/" + path);
	
	if (isMipmap)
	{
#ifdef GL_VERSION_2_0
		glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8,
						  image.getSize().width, image.getSize().height,
						  getGLFormat(image.getFormat()),
						  GL_UNSIGNED_BYTE, image.getPixels());
#endif
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
					 image.getSize().width, image.getSize().height,
					 0, getGLFormat(image.getFormat()),
					 GL_UNSIGNED_BYTE, image.getPixels());
	}
	
	textureSize[textureIndex] = image.getSize();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLCanvas::updateTextureSize(int textureIndex, OESize size)
{
	if (size.width < 4)
		size.width = 4;
	if (size.height < 1)
		size.height = 1;
	
	OESize texSize = OEMakeSize(getNextPowerOf2(size.width),
								getNextPowerOf2(size.height));
	
	if ((textureSize[textureIndex].width == texSize.width) &&
		(textureSize[textureIndex].height == texSize.height))
		return;
	
	textureSize[textureIndex] = texSize;
	
	vector<char> dummy;
	dummy.resize(texSize.width * texSize.height);
	
	glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
				 texSize.width, texSize.height, 0,
				 GL_LUMINANCE, GL_UNSIGNED_BYTE, &dummy.front());
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLCanvas::loadShaders()
{
	deleteShaders();
	
	shader[OPENGLCANVAS_SHADER_RGB] = loadShader("\
		uniform sampler2D texture;\
		uniform vec2 texture_size;\
		uniform vec3 c0, c1, c2, c3, c4, c5, c6, c7, c8;\
		uniform mat3 decoder;\
		\
		vec3 pixel(vec2 q)\
		{\
		return texture2D(texture, q).rgb;\
		}\
		\
		vec3 pixels(vec2 q, float i)\
		{\
		return pixel(vec2(q.x + i, q.y)) + pixel(vec2(q.x - i, q.y));\
		}\
		\
		void main(void)\
		{\
		vec2 q = gl_TexCoord[0].xy;\
		vec3 p = pixel(q) * c0;\
		p += pixels(q, 1.0 / texture_size.x) * c1;\
		p += pixels(q, 2.0 / texture_size.x) * c2;\
		p += pixels(q, 3.0 / texture_size.x) * c3;\
		p += pixels(q, 4.0 / texture_size.x) * c4;\
		p += pixels(q, 5.0 / texture_size.x) * c5;\
		p += pixels(q, 6.0 / texture_size.x) * c6;\
		p += pixels(q, 7.0 / texture_size.x) * c7;\
		p += pixels(q, 8.0 / texture_size.x) * c8;\
		gl_FragColor = vec4(decoder * p, 1.0);\
		}");
	
	shader[OPENGLCANVAS_SHADER_NTSC] = loadShader("\
		uniform sampler2D texture;\
		uniform vec2 texture_size;\
		uniform vec2 comp_phase;\
		uniform float comp_black;\
		uniform vec3 c0, c1, c2, c3, c4, c5, c6, c7, c8;\
		uniform mat3 decoder;\
		\
		float PI = 3.14159265358979323846264;\
		\
		vec3 pixel(vec2 q)\
		{\
			vec3 p = texture2D(texture, q).rgb - comp_black;\
			float phase = 2.0 * PI * dot(comp_phase * texture_size, q);\
			return p * vec3(1.0, sin(phase), cos(phase));\
		}\
		\
		vec3 pixels(vec2 q, float i)\
		{\
			return pixel(vec2(q.x + i, q.y)) + pixel(vec2(q.x - i, q.y));\
		}\
		\
		void main(void)\
		{\
			vec2 q = gl_TexCoord[0].xy;\
			vec3 p = pixel(q) * c0;\
			p += pixels(q, 1.0 / texture_size.x) * c1;\
			p += pixels(q, 2.0 / texture_size.x) * c2;\
			p += pixels(q, 3.0 / texture_size.x) * c3;\
			p += pixels(q, 4.0 / texture_size.x) * c4;\
			p += pixels(q, 5.0 / texture_size.x) * c5;\
			p += pixels(q, 6.0 / texture_size.x) * c6;\
			p += pixels(q, 7.0 / texture_size.x) * c7;\
			p += pixels(q, 8.0 / texture_size.x) * c8;\
			gl_FragColor = vec4(decoder * p, 1.0);\
		}");
	
	shader[OPENGLCANVAS_SHADER_PAL] = loadShader("\
		uniform sampler2D texture;\
		uniform vec2 texture_size;\
		uniform vec2 comp_phase;\
		uniform float comp_black;\
		uniform vec3 c0, c1, c2, c3, c4, c5, c6, c7, c8;\
		uniform mat3 decoder;\
		\
		float PI = 3.14159265358979323846264;\
		\
		vec3 pixel(vec2 q)\
		{\
			vec3 p = texture2D(texture, q).rgb - comp_black;\
			float phase = 2.0 * PI * dot(comp_phase * texture_size, q);\
			float pal = -sqrt(2.0) * sin(0.5 * PI * texture_size.y * q.y);\
			return p * vec3(1.0, sin(phase), cos(phase) * pal);\
		}\
		\
		vec3 pixels(vec2 q, float i)\
		{\
			return pixel(vec2(q.x + i, q.y)) + pixel(vec2(q.x - i, q.y));\
		}\
		\
		void main(void)\
		{\
			vec2 q = gl_TexCoord[0].xy;\
			vec3 p = pixel(q) * c0;\
			p += pixels(q, 1.0 / texture_size.x) * c1;\
			p += pixels(q, 2.0 / texture_size.x) * c2;\
			p += pixels(q, 3.0 / texture_size.x) * c3;\
			p += pixels(q, 4.0 / texture_size.x) * c4;\
			p += pixels(q, 5.0 / texture_size.x) * c5;\
			p += pixels(q, 6.0 / texture_size.x) * c6;\
			p += pixels(q, 7.0 / texture_size.x) * c7;\
			p += pixels(q, 8.0 / texture_size.x) * c8;\
			gl_FragColor = vec4(decoder * p, 1.0);\
		}");
	
	shader[OPENGLCANVAS_SHADER_DISPLAY] = loadShader("\
		uniform sampler2D texture;\
		uniform vec2 texture_size;\
		uniform float barrel;\
		uniform vec2 barrel_center;\
		uniform float scanline_alpha;\
		uniform float center_lighting;\
		uniform sampler2D shadowmask;\
		uniform vec2 shadowmask_scale;\
		uniform vec2 shadowmask_translate;\
		uniform float shadowmask_alpha;\
		uniform float brightness;\
		uniform float alpha;\
		\
		float PI = 3.14159265358979323846264;\
		\
		void main(void)\
		{\
			vec2 q = gl_TexCoord[0].xy;\
			\
			vec2 qc = q - barrel_center;\
			q += barrel * qc * dot(qc, qc);\
			\
			vec3 p = texture2D(texture, q).rgb;\
			float s = sin(PI * texture_size.y * q.y);\
			p *= mix(1.0, s * s, scanline_alpha);\
			vec2 c = qc * center_lighting;\
			p *= exp(-dot(c, c));\
			vec3 m = texture2D(shadowmask, q * shadowmask_scale +\
							   shadowmask_translate).rgb;\
			p *= mix(vec3(1.0, 1.0, 1.0), m, shadowmask_alpha);\
			p += brightness;\
			gl_FragColor = vec4(p, alpha);\
		}");
}

void OpenGLCanvas::deleteShaders()
{
	deleteShader(OPENGLCANVAS_SHADER_NTSC);
	deleteShader(OPENGLCANVAS_SHADER_PAL);
	deleteShader(OPENGLCANVAS_SHADER_RGB);
	deleteShader(OPENGLCANVAS_SHADER_DISPLAY);
}

GLuint OpenGLCanvas::loadShader(const char *source)
{
	GLuint index = 0;
	
#ifdef GL_VERSION_2_0
	const GLchar **sourcePointer = (const GLchar **) &source;
	GLint sourceLength = strlen(source);
	
	GLuint glFragmentShader;
	glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glFragmentShader, 1, sourcePointer, &sourceLength);
	glCompileShader(glFragmentShader);
	
	GLint bufSize;
	
	bufSize = 0;
	glGetShaderiv(glFragmentShader, GL_INFO_LOG_LENGTH, &bufSize);
	
	if (bufSize > 0)
	{
		GLsizei infoLogLength = 0;
		
		vector<char> infoLog;
		infoLog.resize(bufSize);
		
		glGetShaderInfoLog(glFragmentShader, bufSize,
						   &infoLogLength, &infoLog.front());
		
		string errorString = "could not compile OpenGL fragment shader\n";
		errorString += &infoLog.front();
		
		printLog(errorString);
	}
	
	index = glCreateProgram();
	glAttachShader(index, glFragmentShader);
	glLinkProgram(index);
	
	glDeleteShader(glFragmentShader);
	
	bufSize = 0;
	glGetProgramiv(index, GL_INFO_LOG_LENGTH, &bufSize);
	
	if (bufSize > 0)
	{
		GLsizei infoLogLength = 0;
		
		vector<char> infoLog;
		infoLog.resize(bufSize);
		
		glGetProgramInfoLog(index, bufSize,
							&infoLogLength, &infoLog.front());
		
		string errorString = "could not link OpenGL program\n";
		errorString += &infoLog.front();
		
		printLog(errorString);
	}
#endif
	
	return index;
}

void OpenGLCanvas::deleteShader(GLuint index)
{
#ifdef GL_VERSION_2_0
	if (shader[index])
		glDeleteProgram(shader[index]);
	shader[index] = 0;
#endif
}

// Display canvas

bool OpenGLCanvas::uploadImage()
{
	updateTextureSize(OPENGLCANVAS_TEXTURE_IMAGE_RAW, image.getSize());
	
	glBindTexture(GL_TEXTURE_2D, texture[OPENGLCANVAS_TEXTURE_IMAGE_RAW]);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
					image.getSize().width, image.getSize().height,
					getGLFormat(image.getFormat()), GL_UNSIGNED_BYTE,
					image.getPixels());
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}

void OpenGLCanvas::updateDisplayConfiguration()
{
	isShaderActive = false;
	
#ifdef GL_VERSION_2_0
	// Deactivate shader when needed
	if (!isShaderEnabled)
		return;
	if (mode == CANVAS_MODE_PAPER)
		return;
	
	// Y'UV filters
	OEVector w = OEVector::chebyshevWindow(17, 50);
	w = w.normalize();
	
	OEVector wy;
	wy = w * OEVector::lanczosWindow(17, displayConfiguration.videoBandwidth);
	wy = wy.normalize();
	
	OEVector wu, wv;
	switch (displayConfiguration.videoDecoder)
	{
		case CANVAS_DECODER_RGB:
		case CANVAS_DECODER_MONOCHROME:
			wu = wv = wy;
			break;
			
		case CANVAS_DECODER_NTSC_YIQ:
			wu = w * OEVector::lanczosWindow(17, (displayConfiguration.
												  compositeChromaBandwidth));
			wu = wu.normalize() * 2;
			wv = w * OEVector::lanczosWindow(17, (displayConfiguration.
												  compositeChromaBandwidth +
												  NTSC_YIQ_I_SHIFT));
			wv = wv.normalize() * 2;
			break;
			
		default:
			wu = w * OEVector::lanczosWindow(17, (displayConfiguration.
												  compositeChromaBandwidth));
			wu = wv = wu.normalize() * 2;
			break;
	}
	
	// Decoder matrix
	OEMatrix3 m(1, 0, 0,
				0, 1, 0,
				0, 0, 1);
	
	// Contrast
	m *= displayConfiguration.videoContrast;
	
	// Decoder matrices from "Digital Video and HDTV Algorithms and Interfaces"
	switch (displayConfiguration.videoDecoder)
	{
		case CANVAS_DECODER_RGB:
		case CANVAS_DECODER_MONOCHROME:
			// Y'PbPr decoder matrix
			m *= OEMatrix3(1, 1, 1,
						   0, -0.344, 1.772,
						   1.402, -0.714, 0);
			break;
			
		case CANVAS_DECODER_NTSC_YUV:
		case CANVAS_DECODER_NTSC_YIQ:
			// Y'IQ decoder matrix
			m *= OEMatrix3(1, 1, 1,
						   0.955986, -0.272013, -1.106740,
						   0.620825, -0.647204, 1.704230);
			// Invert IQ
			m *= OEMatrix3(1, 0, 0,
						   0, 0, 1,
						   0, 1, 0);
			break;
			
		case CANVAS_DECODER_NTSC_CXA2025AS:
			// CXA2025AS decoder matrix
			m *= OEMatrix3(1, 1, 1,
						   1.630, -0.378, -1.089,
						   0.317, -0.466, 1.677);
			// Invert IQ
			m *= OEMatrix3(1, 0, 0,
						   0, 0, 1,
						   0, 1, 0);
			break;
			
		case CANVAS_DECODER_PAL:
			// Y'UV decoder matrix
			m *= OEMatrix3(1, 1, 1,
						   0, -0.394642, 2.032062,
						   1.139883, -0.580622, 0);
			break;
	}
	
	// Hue
	float hue = 2 * M_PI * displayConfiguration.videoHue;
	m *= OEMatrix3(1, 0, 0,
				   0, cosf(hue), -sinf(hue),
				   0, sinf(hue), cosf(hue));
	
	// Saturation
	m *= OEMatrix3(1, 0, 0,
				   0, displayConfiguration.videoSaturation, 0,
				   0, 0, displayConfiguration.videoSaturation);
	
	// Encoder matrices
	switch (displayConfiguration.videoDecoder)
	{
		case CANVAS_DECODER_RGB:
			// Y'PbPr encoding matrix
			m *= OEMatrix3(0.299, -0.169, 0.5,
						   0.587, -0.331, -0.419,
						   0.114, 0.5, -0.081);
			break;
			
		case CANVAS_DECODER_MONOCHROME:
			// Set Y'PbPr maximum hue
			m *= OEMatrix3(1, 0, 0,
						   0, 0.5, 0,
						   0, 0, 0);
			break;
	}
	
	// Dynamic range gain
	switch (displayConfiguration.videoDecoder)
	{
		case CANVAS_DECODER_NTSC_YIQ:
		case CANVAS_DECODER_NTSC_CXA2025AS:
		case CANVAS_DECODER_NTSC_YUV:
		case CANVAS_DECODER_PAL:
			float levelRange = (displayConfiguration.compositeWhiteLevel -
								displayConfiguration.compositeBlackLevel);
			if (fabs(levelRange) < 0.01)
				levelRange = 0.01;
			m *= 1 / levelRange;
			break;
	}
	
	switch (displayConfiguration.videoDecoder)
	{
		case CANVAS_DECODER_NTSC_YIQ:
		case CANVAS_DECODER_NTSC_YUV:
		case CANVAS_DECODER_NTSC_CXA2025AS:
			renderShader = shader[OPENGLCANVAS_SHADER_NTSC];
			break;
		case CANVAS_DECODER_PAL:
			renderShader = shader[OPENGLCANVAS_SHADER_PAL];
			break;
		default:
			renderShader = shader[OPENGLCANVAS_SHADER_RGB];
			break;
	}
	
	isShaderActive = renderShader && shader[OPENGLCANVAS_SHADER_DISPLAY];
	if (!isShaderActive)
		return;
	
	glUseProgram(renderShader);
	
	if (renderShader != shader[OPENGLCANVAS_SHADER_RGB])
	{
		glUniform2f(glGetUniformLocation(renderShader, "comp_phase"),
					displayConfiguration.compositeCarrierFrequency,
					displayConfiguration.compositeLinePhase);
		glUniform1f(glGetUniformLocation(renderShader, "comp_black"),
					displayConfiguration.compositeBlackLevel);
	}
	glUniform3f(glGetUniformLocation(renderShader, "c0"),
				wy.getValue(8), wu.getValue(8), wv.getValue(8));
	glUniform3f(glGetUniformLocation(renderShader, "c1"),
				wy.getValue(7), wu.getValue(7), wv.getValue(7));
	glUniform3f(glGetUniformLocation(renderShader, "c2"),
				wy.getValue(6), wu.getValue(6), wv.getValue(6));
	glUniform3f(glGetUniformLocation(renderShader, "c3"),
				wy.getValue(5), wu.getValue(5), wv.getValue(5));
	glUniform3f(glGetUniformLocation(renderShader, "c4"),
				wy.getValue(4), wu.getValue(4), wv.getValue(4));
	glUniform3f(glGetUniformLocation(renderShader, "c5"),
				wy.getValue(3), wu.getValue(3), wv.getValue(3));
	glUniform3f(glGetUniformLocation(renderShader, "c6"),
				wy.getValue(2), wu.getValue(2), wv.getValue(2));
	glUniform3f(glGetUniformLocation(renderShader, "c7"),
				wy.getValue(1), wu.getValue(1), wv.getValue(1));
	glUniform3f(glGetUniformLocation(renderShader, "c8"),
				wy.getValue(0), wu.getValue(0), wv.getValue(0));
	glUniformMatrix3fv(glGetUniformLocation(renderShader, "decoder"),
					   9, false, m.getValues());
	
	GLuint displayShader = shader[OPENGLCANVAS_SHADER_DISPLAY];
	glUseProgram(displayShader);
	glUniform1i(glGetUniformLocation(displayShader, "shadowmask"), 1);
	glUniform1f(glGetUniformLocation(displayShader, "barrel"),
				displayConfiguration.displayBarrel);
	float centerLighting = displayConfiguration.displayCenterLighting;
	if (fabs(centerLighting) < 0.001)
		centerLighting = 0.001;
	glUniform1f(glGetUniformLocation(displayShader, "center_lighting"),
				1.0 / centerLighting - 1.0);
	glUniform1f(glGetUniformLocation(displayShader, "brightness"),
				displayConfiguration.videoBrightness);
	
	glUseProgram(0);
#endif
}

void OpenGLCanvas::renderImage()
{
#ifdef GL_VERSION_2_0
	renderIndex++;
	if (renderIndex >= OPENGLCANVAS_PERSISTANCE_IMAGE_NUM)
		renderIndex = 0;
	persistance[0] = renderIndex;
	
	if (!isShaderActive)
		return;
	
	GLuint textureIndex = OPENGLCANVAS_TEXTURE_IMAGE_RENDERED + renderIndex;
	OESize texSize = textureSize[OPENGLCANVAS_TEXTURE_IMAGE_RAW];
	updateTextureSize(textureIndex, texSize);
	
	glUseProgram(renderShader);
	glUniform1i(glGetUniformLocation(renderShader, "texture"), 0);
	glUniform2f(glGetUniformLocation(renderShader, "texture_size"),
				texSize.width, texSize.height);
	glReadBuffer(GL_BACK);
	
	OESize imageSize = image.getSize();
	for (float y = 0; y < imageSize.height; y += viewportSize.height)
		for (float x = 0; x < imageSize.width; x += viewportSize.width)
		{
			// Calculate rects
			OESize clipSize = viewportSize;
			if ((x + clipSize.width) > imageSize.width)
				clipSize.width = imageSize.width - x;
			if ((y + clipSize.height) > imageSize.height)
				clipSize.height = imageSize.height - y;
			
			OERect textureRect = OEMakeRect(x / texSize.width,
											y / texSize.height,
											clipSize.width / texSize.width,
											clipSize.height / texSize.height);
			OERect vertexRect = OEMakeRect(-1,
										   -1,
										   2 * clipSize.width / viewportSize.width, 
										   2 * clipSize.height / viewportSize.height);
			
			// Render
			glBindTexture(GL_TEXTURE_2D, texture[OPENGLCANVAS_TEXTURE_IMAGE_RAW]);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glLoadIdentity();
			
			glBegin(GL_QUADS);
			glTexCoord2f(OEMinX(textureRect), OEMinY(textureRect));
			glVertex2f(OEMinX(vertexRect), OEMinY(vertexRect));
			glTexCoord2f(OEMaxX(textureRect), OEMinY(textureRect));
			glVertex2f(OEMaxX(vertexRect), OEMinY(vertexRect));
			glTexCoord2f(OEMaxX(textureRect), OEMaxY(textureRect));
			glVertex2f(OEMaxX(vertexRect), OEMaxY(vertexRect));
			glTexCoord2f(OEMinX(textureRect), OEMaxY(textureRect));
			glVertex2f(OEMinX(vertexRect), OEMaxY(vertexRect));
			glEnd();
			
			// Copy framebuffer
			glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
								x,
								y,
								0,
								0,
								clipSize.width,
								clipSize.height);
		}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glUseProgram(0);
#endif
}

bool OpenGLCanvas::isPersistanceDrawRequired()
{
	for (int i = 1; i < OPENGLCANVAS_PERSISTANCE_IMAGE_NUM; i++)
	{
		if (persistance[i] != persistance[0])
			return true;
	}
	
	return false;
}

void OpenGLCanvas::drawDisplayCanvas()
{
	// Clear
	float clearColor = isShaderActive ? displayConfiguration.videoBrightness : 0;
	glClearColor(clearColor, clearColor, clearColor, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	OESize imageSize = image.getSize();
	if ((imageSize.width == 0) || (imageSize.height == 0))
		return;
	
	// Calculate rects
	OESize texSize = textureSize[OPENGLCANVAS_TEXTURE_IMAGE_RAW];
	OERect videoRect = displayConfiguration.videoRect;
	
	OERect vertexRect = OEMakeRect(2 * videoRect.origin.x - 1,
								   2 * videoRect.origin.y - 1,
								   2 * videoRect.size.width,
								   2 * videoRect.size.height);
	OERect textureRect = OEMakeRect(0,
									0,
									imageSize.width / texSize.width, 
									imageSize.height / texSize.height);
	
	OESize displayResolution = displayConfiguration.displayResolution;
	
	float viewportAspectRatio = viewportSize.width / viewportSize.height;
	float displayAspectRatio = displayResolution.width / displayResolution.height;
	
	float ratio = viewportAspectRatio / displayAspectRatio;
	
	if (ratio > 1)
	{
		vertexRect.origin.x /= ratio;
		vertexRect.size.width /= ratio;
	}
	else
	{
		vertexRect.origin.y *= ratio;
		vertexRect.size.height *= ratio;
	}
	
	// Set shader uniforms
#ifdef GL_VERSION_2_0
	if (isShaderActive)
	{
		GLuint displayShader = shader[OPENGLCANVAS_SHADER_DISPLAY];
		glUseProgram(displayShader);
		
		OEPoint barrelCenter;
		barrelCenter.x = ((0.5 - displayConfiguration.videoRect.origin.x) /
						  displayConfiguration.videoRect.size.width *
						  imageSize.width / texSize.width);
		barrelCenter.y = ((0.5 - displayConfiguration.videoRect.origin.y) /
						  displayConfiguration.videoRect.size.height *
						  imageSize.height / texSize.height);
		glUniform2f(glGetUniformLocation(displayShader, "barrel_center"),
					barrelCenter.x, barrelCenter.y);
		
		GLuint shadowMaskTexture = 0;
		float shadowVerticalScale;
		switch (displayConfiguration.displayShadowMask)
		{
			case CANVAS_SHADOWMASK_TRIAD:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_TRIAD;
				shadowVerticalScale = (240.0 / 274.0);
				break;
			case CANVAS_SHADOWMASK_INLINE:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_INLINE;
				shadowVerticalScale = 1;
				break;
			case CANVAS_SHADOWMASK_APERTURE:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_APERTURE;
				shadowVerticalScale = 1;
				break;
			case CANVAS_SHADOWMASK_LCD:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_LCD;
				shadowVerticalScale = 1;
				break;
			case CANVAS_SHADOWMASK_BAYER:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_BAYER;
				shadowVerticalScale = 1;
				break;
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[shadowMaskTexture]);
		glActiveTexture(GL_TEXTURE0);
		
		float scanlineHeight = (viewportSize.height / imageSize.height *
								displayConfiguration.videoRect.size.height);
		float alpha = displayConfiguration.displayScanlineAlpha;
		float scanlineAlpha = ((scanlineHeight > 2.5) ? alpha :
							   (scanlineHeight < 2) ? 0 :
							   (scanlineHeight - 2) / (2.5 - 2) * alpha);
		glUniform1f(glGetUniformLocation(displayShader, "scanline_alpha"),
					scanlineAlpha);
		
		float shadowMaskAlpha = displayConfiguration.displayShadowMaskAlpha;
		glUniform1f(glGetUniformLocation(displayShader, "shadowmask_alpha"),
					shadowMaskAlpha);
		
		float dotPitch = displayConfiguration.displayShadowMaskDotPitch;
		if (dotPitch <= 0.001)
			dotPitch = 0.001;
		OESize elemNum = OEMakeSize(displayConfiguration.displayResolution.width /
									displayConfiguration.displayPixelDensity *
									25.4 / dotPitch * 0.5,
									displayConfiguration.displayResolution.height / 
									displayConfiguration.displayPixelDensity *
									25.4 / dotPitch * shadowVerticalScale);
		glUniform2f(glGetUniformLocation(displayShader, "shadowmask_scale"),
					texSize.width / imageSize.width *
					displayConfiguration.videoRect.size.width * elemNum.width,
					texSize.height / imageSize.height *
					displayConfiguration.videoRect.size.height * elemNum.height);
		glUniform2f(glGetUniformLocation(displayShader, "shadowmask_translate"),
					displayConfiguration.videoRect.origin.x * elemNum.width,
					displayConfiguration.videoRect.origin.y * elemNum.height);
	}
#endif
	
	// Render
	glLoadIdentity();
	glRotatef(180, 1, 0, 0);
	
	glClearColor(1, 1, 1, 1);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);
	
	int startIndex = ((isShaderActive &&
					   (displayConfiguration.displayPersistance != 0)) ?
					  (OPENGLCANVAS_PERSISTANCE_IMAGE_NUM - 1) : 0);
	for (int i = startIndex; i >= 0; i--)
	{
		if (persistance[i] < 0)
			continue;
		
		int textureIndex;
		if (isShaderActive)
			textureIndex = OPENGLCANVAS_TEXTURE_IMAGE_RENDERED + persistance[i];
		else
			textureIndex = OPENGLCANVAS_TEXTURE_IMAGE_RAW;
		glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
		
#ifdef GL_VERSION_2_0
		if (isShaderActive)
		{
			GLuint displayShader = shader[OPENGLCANVAS_SHADER_DISPLAY];
			OESize size = textureSize[textureIndex];
			glUseProgram(displayShader);
			glUniform1i(glGetUniformLocation(displayShader, "texture"), 0);
			glUniform2f(glGetUniformLocation(displayShader, "texture_size"),
						size.width, size.height);
			glUniform1f(glGetUniformLocation(displayShader, "alpha"),
						displayConfiguration.displayPersistance);
		}
#endif
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		
		// Render
		glBegin(GL_QUADS);
		glTexCoord2f(OEMinX(textureRect), OEMinY(textureRect));
		glVertex2f(OEMinX(vertexRect), OEMinY(vertexRect));
		glTexCoord2f(OEMaxX(textureRect), OEMinY(textureRect));
		glVertex2f(OEMaxX(vertexRect), OEMinY(vertexRect));
		glTexCoord2f(OEMaxX(textureRect), OEMaxY(textureRect));
		glVertex2f(OEMaxX(vertexRect), OEMaxY(vertexRect));
		glTexCoord2f(OEMinX(textureRect), OEMaxY(textureRect));
		glVertex2f(OEMinX(vertexRect), OEMaxY(vertexRect));
		glEnd();
	}
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 0);
	
#ifdef GL_VERSION_2_0
	if (isShaderActive)
		glUseProgram(0);
#endif
}

void OpenGLCanvas::updatePersistance()
{
	for (int i = (OPENGLCANVAS_PERSISTANCE_IMAGE_NUM - 1); i > 0; i--)
		persistance[i] = persistance[i - 1];
}

// Paper canvas

void OpenGLCanvas::drawPaperCanvas()
{
	// Clear
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	OESize imageSize = image.getSize();
	if ((imageSize.width == 0) || (imageSize.height == 0))
		return;
	
	// Render
	float pixelDensityRatio = (paperConfiguration.pagePixelDensity.width /
							   paperConfiguration.pagePixelDensity.height);
	float canvasViewportHeight = (viewportSize.height * imageSize.width /
								  viewportSize.width / pixelDensityRatio);
	OERect viewportCanvas = OEMakeRect(0,
									   clipOrigin.y,
									   imageSize.width,
									   canvasViewportHeight);
	
	OESize texSize = OEMakeSize(getNextPowerOf2(imageSize.width),
								getNextPowerOf2(PAPER_SLICE));
	updateTextureSize(OPENGLCANVAS_TEXTURE_IMAGE_RAW, texSize);
	
	glLoadIdentity();
	glRotatef(180, 1, 0, 0);
	
	int startIndex = OEMinY(viewportCanvas) / PAPER_SLICE;
	int endIndex = OEMaxY(viewportCanvas) / PAPER_SLICE;
	for (int i = startIndex; i <= endIndex; i++)
	{
		OERect slice = OEMakeRect(0,
								  i * PAPER_SLICE,
								  imageSize.width,
								  PAPER_SLICE);
		if (OEMaxY(slice) >= imageSize.height)
			break;
		if (OEMinY(slice) >= imageSize.height)
			slice.size.height = imageSize.height - slice.origin.y;
		
		glBindTexture(GL_TEXTURE_2D, texture[OPENGLCANVAS_TEXTURE_IMAGE_RAW]);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
						slice.size.width, slice.size.height,
						getGLFormat(image.getFormat()), GL_UNSIGNED_BYTE,
						image.getPixels() + image.getBytesPerRow() *
						(int)OEMinY(slice));
		
		OERect textureRect = OEMakeRect(0, 0,
										OEWidth(slice) / texSize.width,
										OEHeight(slice) / texSize.height);
		OERect vertexRect = OEMakeRect(-1,
									   2 * (OEMinY(slice) -
											OEMinY(viewportCanvas)) /
									   OEHeight(viewportCanvas) - 1,
									   2,
									   2 * slice.size.height /
									   OEHeight(viewportCanvas));
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glBegin(GL_QUADS);
		glTexCoord2f(OEMinX(textureRect), OEMinY(textureRect));
		glVertex2f(OEMinX(vertexRect), OEMinY(vertexRect));
		glTexCoord2f(OEMaxX(textureRect), OEMinY(textureRect));
		glVertex2f(OEMaxX(vertexRect), OEMinY(vertexRect));
		glTexCoord2f(OEMaxX(textureRect), OEMaxY(textureRect));
		glVertex2f(OEMaxX(vertexRect), OEMaxY(vertexRect));
		glTexCoord2f(OEMinX(textureRect), OEMaxY(textureRect));
		glVertex2f(OEMinX(vertexRect), OEMaxY(vertexRect));
		glEnd();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Render page separators
	glColor4f(0.8, 0.8, 0.8, 1);
	
	int pageNum = ceil(imageSize.height / paperConfiguration.pageResolution.height);
	for (int i = 0; i < pageNum; i++)
	{
		OERect line = OEMakeRect(-1, 2 * (paperConfiguration.pageResolution.height *
										  (i + 1) - OEMinY(viewportCanvas)) /
								 OEHeight(viewportCanvas) - 1,
								 2, 0);
		if (line.origin.y < -1)
			continue;
		if (line.origin.y > 1)
			break;
		
		glBegin(GL_LINES);
		glVertex2f(OEMinX(line), OEMinY(line));
		glVertex2f(OEMaxX(line), OEMaxY(line));
		glEnd();
	}
	
	glColor4f(1, 1, 1, 1);
}

// Bezel

double OpenGLCanvas::getCurrentTime()
{
	timeval time;
	
	gettimeofday(&time, NULL);
	
	return time.tv_sec + time.tv_usec * (1.0 / 1000000.0);
}

void OpenGLCanvas::drawBezel()
{
	GLuint textureIndex = 0;
	GLfloat textureAlpha = 1;
	GLfloat blackAlpha = 0;
	
	if (isBezelCapture)
	{
		double now = getCurrentTime();
		double diff = now - bezelCaptureTime;
		
		textureIndex = OPENGLCANVAS_TEXTURE_BEZEL_CAPTURE;
		if (diff > (BEZELCAPTURE_START +
					BEZELCAPTURE_FADEOUT))
		{
			isBezelDrawRequired = false;
			isBezelCapture = false;
			textureAlpha = 0;
		}
		else if (diff > BEZELCAPTURE_START)
			textureAlpha = 0.5 + 0.5 * cos((diff - BEZELCAPTURE_START) *
										   M_PI / BEZELCAPTURE_FADEOUT);
	}
	else if (bezel == CANVAS_BEZEL_POWER)
	{
		textureIndex = OPENGLCANVAS_TEXTURE_BEZEL_POWER;
		blackAlpha = 0.3;
		isBezelDrawRequired = false;
	}
	else if (bezel == CANVAS_BEZEL_PAUSE)
	{
		textureIndex = OPENGLCANVAS_TEXTURE_BEZEL_PAUSE;
		blackAlpha = 0.3;
		isBezelDrawRequired = false;
	}
	
	if (!textureIndex)
		return;
	
	// Calculate rects
	OESize size = textureSize[textureIndex];
	
	OERect renderFrame = OEMakeRect(-(size.width + 0.5) / viewportSize.width,
									-(size.height + 0.5) / viewportSize.height,
									2 * size.width / viewportSize.width,
									2 * size.height / viewportSize.height);
	
	// Render
	glLoadIdentity();
	glRotatef(180, 1, 0, 0);
	
	glColor4f(0, 0, 0, blackAlpha);
	
	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
	
	// Render bezel
	glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
	glColor4f(1, 1, 1, textureAlpha);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(OEMinX(renderFrame), OEMinY(renderFrame));
	glTexCoord2f(1, 0);
	glVertex2f(OEMaxX(renderFrame), OEMinY(renderFrame));
	glTexCoord2f(1, 1);
	glVertex2f(OEMaxX(renderFrame), OEMaxY(renderFrame));
	glTexCoord2f(0, 1);
	glVertex2f(OEMinX(renderFrame), OEMaxY(renderFrame));
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(1, 1, 1, 1);
}

// Read framebuffer

OEImage OpenGLCanvas::readFramebuffer()
{
	OERect readRect;
	
	readRect = OEMakeRect(0,
						  0,
						  viewportSize.width,
						  viewportSize.height);
	
	OESize displayResolution = displayConfiguration.displayResolution;
	
	float viewportAspectRatio = viewportSize.width / viewportSize.height;
	float displayAspectRatio = displayResolution.width / displayResolution.height;
	
	float ratio = viewportAspectRatio / displayAspectRatio;
	if (ratio > 1)
	{
		readRect.origin.x = readRect.size.width * ((1.0 - 1.0 / ratio) * 0.5);
		readRect.size.width /= ratio;
	}
	else
	{
		readRect.origin.y = readRect.size.width * ((1.0 - ratio) * 0.5);
		readRect.size.height *= ratio;
	}
	
	OEImage image;
	
	image.setFormat(OEIMAGE_RGB);
	image.setSize(readRect.size);
	
	glReadBuffer(GL_FRONT);
	glReadPixels(OEMinX(readRect), OEMinY(readRect),
				 OEWidth(readRect), OEHeight(readRect),
				 GL_RGB, GL_UNSIGNED_BYTE,
				 image.getPixels());
	
	return image;
}

// HID

void OpenGLCanvas::updateCapture(OpenGLCanvasCapture capture)
{
	//	log("updateCapture");
	
	if (this->capture == capture)
		return;
	this->capture = capture;
	
	if (setCapture)
		setCapture(userData, capture);
}

void OpenGLCanvas::becomeKeyWindow()
{
}

void OpenGLCanvas::resignKeyWindow()
{
	resetKeysAndButtons();
	
	ctrlAltWasPressed = false;
	
	updateCapture(OPENGLCANVAS_CAPTURE_NONE);
}

void OpenGLCanvas::setKey(int usageId, bool value)
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
	
	if ((capture == OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR) &&
		!keyDownCount && ctrlAltWasPressed)
	{
		ctrlAltWasPressed = false;
		
		double now = getCurrentTime();
		if ((now - bezelCaptureTime) < BEZELCAPTURE_START)
			bezelCaptureTime = now - BEZELCAPTURE_START;
		
		updateCapture(OPENGLCANVAS_CAPTURE_NONE);
	}
}

void OpenGLCanvas::postHIDNotification(int notification, int usageId, float value)
{
	CanvasHIDNotification data = {usageId, value};
	
	notify(this, notification, &data);
}

void OpenGLCanvas::sendUnicodeKeyEvent(int unicode)
{
	//	log("unicode " + getHexString(unicode));
	
	postHIDNotification(CANVAS_UNICODEKEYBOARD_DID_CHANGE, unicode, 0);
}

void OpenGLCanvas::enterMouse()
{
	mouseEntered = true;
	
	if (captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
		updateCapture(OPENGLCANVAS_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR);
	
	postHIDNotification(CANVAS_POINTER_DID_CHANGE,
						CANVAS_P_PROXIMITY,
						1);
}

void OpenGLCanvas::exitMouse()
{
	mouseEntered = false;
	
	if (captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
		updateCapture(OPENGLCANVAS_CAPTURE_NONE);
	
	postHIDNotification(CANVAS_POINTER_DID_CHANGE,
						CANVAS_P_PROXIMITY,
						0);
}

void OpenGLCanvas::setMousePosition(float x, float y)
{
	if (capture != OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
	{
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_X,
							x);
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_Y,
							y);
	}
}

void OpenGLCanvas::moveMouse(float rx, float ry)
{
	if (capture == OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
	{
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_RELX,
							rx);
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_RELY,
							ry);
	}
}

void OpenGLCanvas::setMouseButton(int index, bool value)
{
	if (index >= CANVAS_MOUSE_BUTTON_NUM)
		return;
	if (mouseButtonDown[index] == value)
		return;
	mouseButtonDown[index] = value;
	
	if (capture == OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_BUTTON1 + index,
							value);
	else if ((captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK) &&
			 (capture == OPENGLCANVAS_CAPTURE_NONE) &&
			 (bezel == CANVAS_BEZEL_NONE) &&
			 (index == 0))
	{
		isBezelDrawRequired = true;
		isBezelCapture = true;
		bezelCaptureTime = getCurrentTime();
		updateCapture(OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR);
	}
	else
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_BUTTON1 + index,
							value);
}

void OpenGLCanvas::sendMouseWheelEvent(int index, float value)
{
	if (capture == OPENGLCANVAS_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR)
		postHIDNotification(CANVAS_MOUSE_DID_CHANGE,
							CANVAS_M_WHEELX + index,
							value);
	else
		postHIDNotification(CANVAS_POINTER_DID_CHANGE,
							CANVAS_P_WHEELX + index,
							value);
}

void OpenGLCanvas::setJoystickButton(int deviceIndex, int index, bool value)
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

void OpenGLCanvas::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_AXIS_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLCanvas::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_HAT_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLCanvas::moveJoystickBall(int deviceIndex, int index, float value)
{
	if (deviceIndex >= CANVAS_JOYSTICK_NUM)
		return;
	if (index >= CANVAS_JOYSTICK_RAXIS_NUM)
		return;
	
	postHIDNotification(CANVAS_JOYSTICK1_DID_CHANGE + deviceIndex,
						CANVAS_J_AXIS1 + index,
						value);
}

void OpenGLCanvas::resetKeysAndButtons()
{
	for (int i = 0; i < CANVAS_KEYBOARD_KEY_NUM; i++)
		setKey(i, false);
	
	for (int i = 0; i < CANVAS_MOUSE_BUTTON_NUM; i++)
		setMouseButton(i, false);
	
	for (int i = 0; i < CANVAS_JOYSTICK_NUM; i++)
		for (int j = 0; j < CANVAS_JOYSTICK_BUTTON_NUM; j++)
			setJoystickButton(i, j, false);
}

void OpenGLCanvas::doCopy(string& value)
{
	notify(NULL, CANVAS_DID_COPY, &value);
}

void OpenGLCanvas::doPaste(string value)
{
	notify(NULL, CANVAS_DID_PASTE, &value);
}

void OpenGLCanvas::doDelete()
{
	notify(NULL, CANVAS_DID_DELETE, NULL);
}

bool OpenGLCanvas::setMode(CanvasMode *mode)
{
	if (!mode)
		return false;
	
	lock();
	
	this->mode = *mode;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::setCaptureMode(CanvasCaptureMode *captureMode)
{
	if (!captureMode)
		return false;
	
	if (this->captureMode == *captureMode)
		return true;
	
	switch (*captureMode)
	{
		case CANVAS_CAPTUREMODE_NO_CAPTURE:
			updateCapture(OPENGLCANVAS_CAPTURE_NONE);
			break;
			
		case CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK:
			updateCapture(OPENGLCANVAS_CAPTURE_NONE);
			break;
			
		case CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER:
			updateCapture(mouseEntered ? 
						  OPENGLCANVAS_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR : 
						  OPENGLCANVAS_CAPTURE_NONE);
			break;
		default:
			return false;
	}
	
	this->captureMode = *captureMode;
	
	return true;
}

bool OpenGLCanvas::setBezel(CanvasBezel *bezel)
{
	if (!bezel)
		return false;
	
	if (this->bezel == *bezel)
		return true;
	
	lock();
	
	this->bezel = *bezel;
	isBezelDrawRequired = true;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::setDisplayConfiguration(CanvasDisplayConfiguration *configuration)
{
	if (!configuration)
		return false;
	
	lock();
	
	isConfigurationUpdated = true;
	displayConfiguration = *configuration;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::setPaperConfiguration(CanvasPaperConfiguration *configuration)
{
	if (!configuration)
		return false;
	
	lock();
	
	isConfigurationUpdated = true;
	paperConfiguration = *configuration;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::setOpenGLConfiguration(CanvasOpenGLConfiguration *configuration)
{
	if (!configuration)
		return false;
	
	lock();
	
	isConfigurationUpdated = true;
	openGLConfiguration = *configuration;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::postImage(OEImage *theImage)
{
	if (!theImage)
		return false;
	
	lock();
	
	switch (mode)
	{
		case CANVAS_MODE_DISPLAY:
			this->image = *theImage;
			isImageUpdated = true;
			break;
		case CANVAS_MODE_PAPER:
			OESize theImageSize = theImage->getSize();
			OESize imageSize = image.getSize();
			OERect aRect = OEMakeRect(0, 0,
									  theImageSize.width, theImageSize.height);
			OERect bRect = OEMakeRect(printHead.x, printHead.y,
									  imageSize.width, imageSize.height);
			OERect unionRect = OEUnionRect(aRect, bRect);
			
			if (OEIsEmptyRect(bRect))
				image.setFormat(theImage->getFormat());
			image.setSize(unionRect.size);
			image.overlay(printHead, *theImage);
			isImageUpdated = true;
			break;
	}
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::setPrintHead(OEPoint *point)
{
	if (!point)
		return false;
	
	lock();
	
	printHead = *point;
	
	unlock();
	
	return true;
}

bool OpenGLCanvas::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case CANVAS_SET_MODE:
			return setMode((CanvasMode *)data);
			
		case CANVAS_SET_CAPTUREMODE:
			return setCaptureMode((CanvasCaptureMode *)data);
			
		case CANVAS_SET_KEYBOARDFLAGS:
			if (setKeyboardFlags)
				setKeyboardFlags(userData, *((OEUInt32 *)data));
			
			return true;
			
		case CANVAS_SET_BEZEL:
			return setBezel((CanvasBezel *)data);
			
		case CANVAS_CONFIGURE_DISPLAY:
			return setDisplayConfiguration((CanvasDisplayConfiguration *)data);
			
		case CANVAS_CONFIGURE_PAPER:
			return setPaperConfiguration((CanvasPaperConfiguration *)data);
			
		case CANVAS_CONFIGURE_OPENGL:
			return setOpenGLConfiguration((CanvasOpenGLConfiguration *)data);
			
		case CANVAS_POST_IMAGE:
			return postImage((OEImage *)data);
			
		case CANVAS_CLEAR:
			return true;
			
		case CANVAS_MOVE_PRINTHEAD:
			return setPrintHead((OEPoint *)data);
	}
	
	return false;
}

void OpenGLCanvas::notify(OEComponent *sender, int notification, void *data)
{
	lock();
	
	for (int i = 0; i < observers[notification].size(); i++)
	{
		unlock();
		
		observers[notification][i]->notify(sender, notification, data);
		
		lock();
	}
	
	unlock();
}

bool OpenGLCanvas::addObserver(OEComponent *observer, int notification)
{
	lock();
	
	bool value = OEComponent::addObserver(observer, notification);
	
	unlock();
	
	return value;
}

bool OpenGLCanvas::removeObserver(OEComponent *observer, int notification)
{
	lock();
	
	bool value = OEComponent::removeObserver(observer, notification);
	
	unlock();
	
	return value;
}
