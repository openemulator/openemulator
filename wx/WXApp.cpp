
/**
 * AppleIIGo
 * wxWidgets main
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "WXApp.h"
#include "WXUIController.h"

#include "wx/wx.h"
#include "wx/glcanvas.h"

#ifdef __WXMAC__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

#ifndef WIN32
#include <unistd.h> // FIXME: ¿This work/necessary in Windows?
//Not necessary, but if it was, it needs to be replaced by process.h AND io.h
#endif

class wxGLCanvasSubClass: public wxGLCanvas {
	void Render();
public:
    wxGLCanvasSubClass(wxFrame* parent);
    void Paintit(wxPaintEvent& event);
protected:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxGLCanvasSubClass, wxGLCanvas)
EVT_PAINT    (wxGLCanvasSubClass::Paintit)
END_EVENT_TABLE()

wxGLCanvasSubClass::wxGLCanvasSubClass(wxFrame *parent)
:wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxDefaultSize, 0,
			wxT("GLCanvas")){
    int argc = 0;
    char** argv = NULL;
	
	/*
	 NOTE: this example uses GLUT in order to have a free teapot model
	 to display, to show 3D capabilities. GLUT, however, seems to cause problems
	 on some systems. If you meet problems, first try commenting out glutInit(),
	 then try comeenting out all glut code
	 */
    glutInit(&argc, argv);
}


void wxGLCanvasSubClass::Paintit(wxPaintEvent& WXUNUSED(event)){
    Render();
}

void wxGLCanvasSubClass::Render()
{
    SetCurrent();
    wxPaintDC(this);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (GLint)GetSize().x, (GLint)GetSize().y);
	
    glBegin(GL_POLYGON);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, -0.5);
	glColor3f(0.4, 0.5, 0.4);
	glVertex2f(0.0, -0.8);
    glEnd();
	
    glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(0.1, 0.1);
	glVertex2f(-0.1, 0.1);
	glVertex2f(-0.1, -0.1);
	glVertex2f(0.1, -0.1);
    glEnd();
	
	// using a little of glut
    glColor4f(0,0,1,1);
    glutWireTeapot(0.4);
	
    glLoadIdentity();
    glColor4f(2,0,1,1);
    glutWireTeapot(0.6);
	// done using glut
	
    glFlush();
    SwapBuffers();
}

IMPLEMENT_APP(WXApp)

bool WXApp::OnInit()
{
    wxFrame *frame = new wxFrame((wxFrame *)NULL, -1, 
								 wxT("Hello GL World"), wxPoint(50,50),
								 wxSize(200,200));
    new wxGLCanvasSubClass(frame);
	frame->Show(TRUE);
	
	frame = new WXUIController();
	frame->Show();
    
    return true;
}
