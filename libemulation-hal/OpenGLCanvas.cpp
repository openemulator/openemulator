
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
	
	isNewFrame = false;
	
	bezel = CANVAS_BEZEL_NONE;
	isDrawBezel = false;
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

OESize OpenGLCanvas::getCanvasSize()
{
	return configuration.size;
}

CanvasMode OpenGLCanvas::getCanvasMode()
{
	return configuration.mode;
}

void OpenGLCanvas::setEnableShader(bool value)
{
	lock();
	isShaderEnabled = value;
	isNewConfiguration = true;
	unlock();
}

bool OpenGLCanvas::update(float width, float height, float offset, bool draw)
{
	CanvasUpdate update;
	
	update.width = width;
	update.height = height;
	update.draw = draw;
	notify(this, CANVAS_WILL_UPDATE, &update);
	draw = update.draw;
	
	if (isNewFrame)
		uploadFrame();
	
	if (isNewConfiguration)
		updateConfiguration();
	
	if ((width != viewportSize.width) ||
		(height != viewportSize.height))
	{
		viewportSize.width = width;
		viewportSize.height = height;
		
		updateViewport();
		draw = true;
	}
	
	if (isNewFrame || isNewConfiguration)
	{
		renderFrame();
		
		isNewFrame = false;
		isNewConfiguration = false;
		
		draw = true;
	}
	
	if (isDrawBezel)
		draw = true;
	
	updatePersistance();
	
	if (draw)
	{
		drawCanvas();
		drawBezel();
	}
	
	update.width = width;
	update.height = height;
	update.draw = draw;
	notify(this, CANVAS_DID_UPDATE, &update);
	draw = update.draw;
	
	return draw;
}

// OpenGL

bool OpenGLCanvas::initOpenGL()
{
	isNewConfiguration = true;
	
	viewportSize = OEMakeSize(0, 0);
	for (int i = 0; i < OPENGLCANVAS_TEXTURE_END; i++)
	{
		texture[i] = 0;
		textureSize[i] = OEMakeSize(0, 0);
	}
	for (int i = 0; i < OPENGLCANVAS_SHADER_END; i++)
		shader[i] = 0;
	isShaderActive = false;
	
	renderIndex = 0;
	
	for (int i = 0; i < OPENGLCANVAS_PERSISTANCE_FRAME_NUM; i++)
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
	loadTexture("Shadow Mask Triad.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_TRIAD);
	loadTexture("Shadow Mask Inline.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_INLINE);
	loadTexture("Shadow Mask Aperture.png", true,
				OPENGLCANVAS_TEXTURE_SHADOWMASK_APERTURE);
	loadTexture("Bezel Power.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_POWER);
	loadTexture("Bezel Pause.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_PAUSE);
	loadTexture("Bezel Capture.png", false,
				OPENGLCANVAS_TEXTURE_BEZEL_CAPTURE);
}

void OpenGLCanvas::loadTexture(string path, bool isMipmap, int textureIndex)
{
	OEImage image;
	image.readFile(resourcePath + "/images/Host/" + path);
	
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
	
	OESize powerOf2Size = OEMakeSize(getNextPowerOf2(size.width),
									 getNextPowerOf2(size.height));
	
	if ((textureSize[textureIndex].width == powerOf2Size.width) &&
		(textureSize[textureIndex].height == powerOf2Size.height))
		return;
	
	textureSize[textureIndex] = powerOf2Size;
	
	vector<char> dummy;
	dummy.resize(powerOf2Size.width * powerOf2Size.height);
	
	glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
				 powerOf2Size.width, powerOf2Size.height, 0,
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
	
	shader[OPENGLCANVAS_SHADER_SCREEN] = loadShader("\
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
			vec3 m = texture2D(shadowmask, q * shadowmask_scale + shadowmask_translate).rgb;\
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
	deleteShader(OPENGLCANVAS_SHADER_SCREEN);
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

bool OpenGLCanvas::uploadFrame()
{
	updateTextureSize(OPENGLCANVAS_TEXTURE_FRAME_RAW, frame.getSize());
	
	glBindTexture(GL_TEXTURE_2D, texture[OPENGLCANVAS_TEXTURE_FRAME_RAW]);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
					frame.getSize().width, frame.getSize().height,
					getGLFormat(frame.getFormat()), GL_UNSIGNED_BYTE,
					frame.getPixels());
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}

void OpenGLCanvas::updateConfiguration()
{
	isShaderActive = false;
#ifdef GL_VERSION_2_0
	// Deactivate shader when needed
	if (!isShaderEnabled)
		return;
	if (configuration.mode == CANVAS_MODE_PAPER)
		return;
	
	// Y'UV filters
	OEVector w = OEVector::chebyshevWindow(17, 50);
	w = w.normalize();
	
	OEVector wy;
	wy = w * OEVector::lanczosWindow(17, configuration.videoBandwidth);
	wy = wy.normalize();
	
	OEVector wu, wv;
	switch (configuration.videoDecoder)
	{
		case CANVAS_DECODER_RGB:
		case CANVAS_DECODER_MONOCHROME:
			wu = wv = wy;
			break;
			
		case CANVAS_DECODER_NTSC_YIQ:
			wu = w * OEVector::lanczosWindow(17, (configuration.
												  compositeChromaBandwidth));
			wu = wu.normalize() * 2;
			wv = w * OEVector::lanczosWindow(17, (configuration.
												  compositeChromaBandwidth +
												  NTSC_YIQ_I_SHIFT));
			wv = wv.normalize() * 2;
			break;
			
		default:
			wu = w * OEVector::lanczosWindow(17, (configuration.
												  compositeChromaBandwidth));
			wu = wv = wu.normalize() * 2;
			break;
	}
	
	// Decoder matrix
	OEMatrix3 m(1, 0, 0,
				0, 1, 0,
				0, 0, 1);
	
	// Contrast
	m *= configuration.videoContrast;
	
	// Decoder matrices from "Digital Video and HDTV Algorithms and Interfaces"
	switch (configuration.videoDecoder)
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
	float hue = 2 * M_PI * configuration.videoHue;
	m *= OEMatrix3(1, 0, 0,
				   0, cosf(hue), -sinf(hue),
				   0, sinf(hue), cosf(hue));
	
	// Saturation
	m *= OEMatrix3(1, 0, 0,
				   0, configuration.videoSaturation, 0,
				   0, 0, configuration.videoSaturation);
	
	// Encoder matrices
	switch (configuration.videoDecoder)
	{
		case CANVAS_DECODER_RGB:
			// Y'PbPr encoding matrix
			m *= OEMatrix3(0.299, -0.169, 0.5,
						   0.587, -0.331, -0.419,
						   0.114, 0.5, -0.081);
			break;
			
		case CANVAS_DECODER_MONOCHROME:
			// Set Y'PbPr maximum hue
			m *= OEMatrix3(1, 0, -0.5,
						   0, 0, 0,
						   0, 0, 0);
			break;
	}
	
	// Dynamic range gain
	switch (configuration.videoDecoder)
	{
		case CANVAS_DECODER_NTSC_YIQ:
		case CANVAS_DECODER_NTSC_CXA2025AS:
		case CANVAS_DECODER_NTSC_YUV:
		case CANVAS_DECODER_PAL:
			float levelRange = (configuration.compositeWhiteLevel -
								configuration.compositeBlackLevel);
			if (fabs(levelRange) < 0.01)
				levelRange = 0.01;
			m *= 1 / levelRange;
			break;
	}
	
	switch (configuration.videoDecoder)
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
	
	isShaderActive = renderShader && shader[OPENGLCANVAS_SHADER_SCREEN];
	if (!isShaderActive)
		return;
	
	glUseProgram(renderShader);
	
	if (renderShader != shader[OPENGLCANVAS_SHADER_RGB])
	{
		glUniform2f(glGetUniformLocation(renderShader, "comp_phase"),
					configuration.compositeCarrierFrequency,
					configuration.compositeLinePhase);
		glUniform1f(glGetUniformLocation(renderShader, "comp_black"),
					configuration.compositeBlackLevel);
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
	
	GLuint screenShader = shader[OPENGLCANVAS_SHADER_SCREEN];
	glUseProgram(screenShader);
	glUniform1i(glGetUniformLocation(screenShader, "shadowmask"), 1);
	glUniform1f(glGetUniformLocation(screenShader, "barrel"),
				configuration.screenBarrel);
	float centerLighting = configuration.screenCenterLighting;
	if (fabs(centerLighting) < 0.001)
		centerLighting = 0.001;
	glUniform1f(glGetUniformLocation(screenShader, "center_lighting"),
				1.0 / centerLighting - 1.0);
	glUniform1f(glGetUniformLocation(screenShader, "brightness"),
				configuration.videoBrightness);
	
	glUseProgram(0);
#endif
}

void OpenGLCanvas::updateViewport()
{
	glViewport(0, 0, viewportSize.width, viewportSize.height);
}

void OpenGLCanvas::renderFrame()
{
#ifdef GL_VERSION_2_0
	if (!isShaderActive)
		return;
	
	renderIndex++;
	if (renderIndex >= OPENGLCANVAS_PERSISTANCE_FRAME_NUM)
		renderIndex = 0;
	
	GLuint textureIndex = OPENGLCANVAS_TEXTURE_FRAME_RENDERED + renderIndex;
	OESize rawTextureSize = textureSize[OPENGLCANVAS_TEXTURE_FRAME_RAW];
	updateTextureSize(textureIndex, rawTextureSize);
	
	glUseProgram(renderShader);
	glUniform1i(glGetUniformLocation(renderShader, "texture"), 0);
	glUniform2f(glGetUniformLocation(renderShader, "texture_size"),
				rawTextureSize.width, rawTextureSize.height);
	
	OESize frameSize = frame.getSize();
	for (float y = 0; y < frameSize.height; y += viewportSize.height)
		for (float x = 0; x < frameSize.width; x += viewportSize.width)
		{
			// Calculate rects
			OESize clipSize = viewportSize;
			if (clipSize.width > frameSize.width)
				clipSize.width = frameSize.width;
			if (clipSize.height > frameSize.height)
				clipSize.height = frameSize.height;
			
			OERect textureRect = OEMakeRect(x / rawTextureSize.width,
											y / rawTextureSize.height,
											clipSize.width / rawTextureSize.width,
											clipSize.height / rawTextureSize.height);
			OERect vertexRect = OEMakeRect(-1, -1,
										   2 * clipSize.width / viewportSize.width, 
										   2 * clipSize.height / viewportSize.height);
			
			// Render
			glBindTexture(GL_TEXTURE_2D, texture[OPENGLCANVAS_TEXTURE_FRAME_RAW]);
			
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
								0, 0,
								clipSize.width,
								clipSize.height);
		}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glUseProgram(0);
#endif
}

void OpenGLCanvas::drawCanvas()
{
	// Clear
	float clearColor;
	if (configuration.mode == CANVAS_MODE_PAPER)
		clearColor = 0x80;
	else
		clearColor = isShaderActive ? configuration.videoBrightness : 0;
	glClearColor(clearColor, clearColor, clearColor, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	OESize size = textureSize[OPENGLCANVAS_TEXTURE_FRAME_RAW];
	if ((size.width == 0) || (size.height == 0))
		return;
	
	// Calculate rects
	OESize frameSize = frame.getSize();
	
	OERect vertexRect = OEMakeRect(2 * configuration.contentRect.origin.x - 1,
								   2 * configuration.contentRect.origin.y - 1,
								   2 * configuration.contentRect.size.width,
								   2 * configuration.contentRect.size.height);
	
	OERect textureRect = OEMakeRect(0, 0,
									frameSize.width / size.width, 
									frameSize.height / size.height);
	
	float viewAspectRatio = viewportSize.width / viewportSize.height;
	float canvasAspectRatio = (configuration.size.width /
							   configuration.size.height);
	
	float ratio = viewAspectRatio / canvasAspectRatio;
	
	if (configuration.mode == CANVAS_MODE_PAPER)
		vertexRect.size.height *= ratio;
	else
	{
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
	}
	
	// Set shader uniforms
#ifdef GL_VERSION_2_0
	if (isShaderActive)
	{
		GLuint screenShader = shader[OPENGLCANVAS_SHADER_SCREEN];
		glUseProgram(screenShader);
		
		OEPoint barrelCenter;
		barrelCenter.x = ((0.5 - configuration.contentRect.origin.x) /
						  configuration.contentRect.size.width *
						  frameSize.width / size.width);
		barrelCenter.y = ((0.5 - configuration.contentRect.origin.y) /
						  configuration.contentRect.size.height *
						  frameSize.height / size.height);
		glUniform2f(glGetUniformLocation(screenShader, "barrel_center"),
					barrelCenter.x, barrelCenter.y);
		
		GLuint shadowMaskTexture = 0;
		switch (configuration.screenShadowMask)
		{
			case CANVAS_SHADOWMASK_TRIAD:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_TRIAD;
				break;
			case CANVAS_SHADOWMASK_INLINE:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_INLINE;
				break;
			case CANVAS_SHADOWMASK_APERTURE:
				shadowMaskTexture = OPENGLCANVAS_TEXTURE_SHADOWMASK_APERTURE;
				break;
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[shadowMaskTexture]);
		glActiveTexture(GL_TEXTURE0);
		
		float scanlineHeight = (viewportSize.height / frameSize.height *
								configuration.contentRect.size.height);
		float alpha = configuration.screenScanlineAlpha;
		float scanlineAlpha = ((scanlineHeight > 2.5) ? alpha :
							   (scanlineHeight < 2) ? 0 :
							   (scanlineHeight - 2) / (2.5 - 2) * alpha);
		glUniform1f(glGetUniformLocation(screenShader, "scanline_alpha"),
					scanlineAlpha);
		
		float shadowMaskAlpha = configuration.screenShadowMaskAlpha;
		glUniform1f(glGetUniformLocation(screenShader, "shadowmask_alpha"),
					shadowMaskAlpha);
		
		// Dot pitch is in mm, at 75 DPI host resolution
		float elemNumX = (configuration.size.width / 75.0 * 25.4 /
						  (configuration.screenShadowMaskDotPitch + 0.001) * 0.5);
		float elemNumY = (configuration.size.height / 75.0 * 25.4 /
						  (configuration.screenShadowMaskDotPitch + 0.001) * (240.0 / 274.0));
		glUniform2f(glGetUniformLocation(screenShader, "shadowmask_scale"),
					size.width / frameSize.width *
					configuration.contentRect.size.width * elemNumX,
					size.height / frameSize.height *
					configuration.contentRect.size.height * elemNumY);
		glUniform2f(glGetUniformLocation(screenShader, "shadowmask_translate"),
					configuration.contentRect.origin.x * elemNumX,
					configuration.contentRect.origin.y * elemNumY);
	}
#endif
	
	// Render
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);
	
	int startIndex = ((isShaderActive && configuration.screenPersistance) ? 
					  (OPENGLCANVAS_PERSISTANCE_FRAME_NUM - 1) : 0);
	for (int i = startIndex; i >= 0; i--)
	{
		if (persistance[i] < 0)
			continue;
		
		int textureIndex;
		if (isShaderActive)
			textureIndex = OPENGLCANVAS_TEXTURE_FRAME_RENDERED + persistance[i];
		else
			textureIndex = OPENGLCANVAS_TEXTURE_FRAME_RAW;
		glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
		
#ifdef GL_VERSION_2_0
		if (isShaderActive)
		{
			GLuint screenShader = shader[OPENGLCANVAS_SHADER_SCREEN];
			OESize size = textureSize[textureIndex];
			glUniform1i(glGetUniformLocation(screenShader, "texture"), 0);
			glUniform2f(glGetUniformLocation(screenShader, "texture_size"),
						size.width, size.height);
			glUniform1f(glGetUniformLocation(screenShader, "alpha"),
						configuration.screenPersistance);
		}
#endif
		
		// Use nearest filter when canvas and screen pixel size match
		int renderFrameWidth = viewportSize.width * vertexRect.size.width / 2;
		GLint param = GL_LINEAR;
		if ((renderFrameWidth == (int) frameSize.width) &&
			(vertexRect.origin.x == -1) && (vertexRect.origin.y == -1))
			param = GL_NEAREST;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		
		// Render
		glLoadIdentity();
		glRotatef(180, 1, 0, 0);
		
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
	for (int i = (OPENGLCANVAS_PERSISTANCE_FRAME_NUM - 1); i > 0; i--)
		persistance[i] = persistance[i - 1];
	
	persistance[0] = renderIndex;
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
			isDrawBezel = false;
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
		isDrawBezel = false;
	}
	else if (bezel == CANVAS_BEZEL_PAUSE)
	{
		textureIndex = OPENGLCANVAS_TEXTURE_BEZEL_PAUSE;
		blackAlpha = 0.3;
		isDrawBezel = false;
	}
	
	if (!textureIndex)
		return;
	
	// Calculate rects
	OESize size = textureSize[textureIndex];
	size.width /= viewportSize.width;
	size.height /= viewportSize.height;
	
	OERect renderFrame = OEMakeRect(-size.width,
									-size.height,
									2 * size.width,
									2 * size.height);
	
	// Render
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
	
	if (configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
		updateCapture(OPENGLCANVAS_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR);
	
	postHIDNotification(CANVAS_POINTER_DID_CHANGE,
						CANVAS_P_PROXIMITY,
						1);
}

void OpenGLCanvas::exitMouse()
{
	mouseEntered = false;
	
	if (configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_ENTER)
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
	else if ((configuration.captureMode == CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK) &&
			 (capture == OPENGLCANVAS_CAPTURE_NONE) &&
			 (bezel == CANVAS_BEZEL_NONE) &&
			 (index == 0))
	{
		isDrawBezel = true;
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

void OpenGLCanvas::copy(string& value)
{
	notify(NULL, CANVAS_DID_COPY, &value);
}

void OpenGLCanvas::paste(string value)
{
	notify(NULL, CANVAS_DID_PASTE, &value);
}

bool OpenGLCanvas::setConfiguration(CanvasConfiguration *configuration)
{
	if (!configuration)
		return false;
	
	if (this->configuration.captureMode != configuration->captureMode)
	{
		switch (configuration->captureMode)
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
		}
	}
	
	lock();
	this->configuration = *configuration;
	isNewConfiguration = true;
	unlock();
	
	return true;
}

bool OpenGLCanvas::postFrame(OEImage *frame)
{
	if (!frame)
		return false;
	
	pthread_mutex_lock(&mutex);
	this->frame = *frame;
	isNewFrame = true;
	pthread_mutex_unlock(&mutex);
	
	return true;
}

bool OpenGLCanvas::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case CANVAS_CONFIGURE:
			return setConfiguration((CanvasConfiguration *)data);
			
		case CANVAS_POST_FRAME:
			return postFrame((OEImage *)data);
			
		case CANVAS_SET_KEYBOARDFLAGS:
			if (setKeyboardFlags)
			{
				setKeyboardFlags(userData, *((int *)data));
				return true;
			}
			break;
			
		case CANVAS_SET_BEZEL:
			if (bezel != *((CanvasBezel *)data))
			{
				bezel = *((CanvasBezel *)data);
				isDrawBezel = true;
			}
			return true;
			
		case CANVAS_LOCK:
			lock();
			return true;
			
		case CANVAS_UNLOCK:
			unlock();
			return false;
	}
	
	return OEComponent::postMessage(sender, message, data);
}
