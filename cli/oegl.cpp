
/**
 * OpenEmulator
 * OpenEmulator/OpenGL interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/OpenGL interface.
 */

#include "oegl.h"

#include "math.h"
#include "GLUT/glut.h"

GLfloat light0_ambient[] =	{0.2, 0.2, 0.2, 1.0};
GLfloat light0_diffuse[] =	{0.0, 0.0, 0.0, 1.0};
GLfloat light1_diffuse[] =	{1.0, 0.0, 0.0, 1.0};
GLfloat light1_position[] =	{1.0, 1.0, 1.0, 0.0};
GLfloat light2_diffuse[] =	{0.0, 1.0, 0.0, 1.0};
GLfloat light2_position[] =	{-1.0, -1.0, 1.0, 0.0};
GLfloat angle1 = 0.0;
GLfloat angle2 = 0.0;

void oeglOpen()
{
	glNewList(1, GL_COMPILE);
	glutSolidIcosahedron();
	glEndList();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(0.5);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0,
				   1.0,
				   1.0,
				   10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.);
 	glTranslatef(0.0, 0.6, -1.0);
	
}

void oeglDraw()
{
	angle1 = (GLfloat) fmod(angle1 + 360.0/200, 360.0);
	
	static GLfloat amb[] = {0.4, 0.4, 0.4, 0.0};
	static GLfloat dif[] = {1.0, 1.0, 1.0, 0.0};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	amb[3] = dif[3] = 2.5;
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	
	glPushMatrix();
	glRotatef(angle1, 1.0, 5.0, 0.0);
	glCallList(1);
	glPopMatrix();
}

void oeglClose()
{
}
