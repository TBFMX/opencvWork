// -- Written in C -- //
#include "wavefrontLoader.h"
#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>


// include generated arrays

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

// OpenGL variables
unsigned int g_persp = 0, g_ortho = 0;

float angle = 0;
//~ void DrawAQuad() {
 //~ glClearColor(1.0, 1.0, 1.0, 1.0);
 //~ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//~ 
//~ // include generated arrays
//~ // set input data to arrays
//~ glVertexPointer(3, GL_FLOAT, 0, bananaVerts);
//~ glNormalPointer(GL_FLOAT, 0, bananaNormals);
//~ glTexCoordPointer(2, GL_FLOAT, 0, bananaTexCoords);
//~ 
//~ glEnableClientState(GL_VERTEX_ARRAY);
    //~ // draw data
//~ glDrawArrays(GL_TRIANGLES, 0, bananaNumVerts);
//~ // deactivate vertex arrays after drawing
//~ glDisableClientState(GL_VERTEX_ARRAY);
//~ 
//~ } 

//~ void DrawAQuad() {
 //~ glClearColor(0.0, 0.0, 0.0, 1.0);
 //~ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//~ 
//~ glEnableClientState(GL_VERTEX_ARRAY);
//~ 
//~ // set input data to arrays
//~ glVertexPointer(3, GL_FLOAT, 0, sedeVerts);
//~ glTexCoordPointer(2, GL_FLOAT, 0, sedeTexCoords);
//~ // draw data
//~ glDrawArrays(GL_TRIANGLES, 0, sedeNumVerts);
//~ // deactivate vertex arrays after drawing
//~ glDisableClientState(GL_VERTEX_ARRAY);
//~ 
//~ } 
WFObject model;

void DrawAQuad() {
	//~ glPushMatrix();
	//~ 
	//~ // clear the screen & depth buffer
	//~ glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//~ 
	//~ // set the perspective projection
	//~ glCallList(g_persp);
//~ 
	//~ // set the camera position
	//~ gluLookAt(	0, 3, 3,	// 	eye pos
				//~ 0, 0, 0,	// 	aim point
				//~ 0, 1, 0);	// 	up direction
//~ 
	//~ // 3D stuff	*****************************************
	//~ glRotatef(angle, 0.0, 1.0, 0.0);
	//~ glColor4f(1.0, 1.0, 1.0, 1.0);
 glClearColor(0.0, 0.0, 0.0, 0.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(-1., 1., -1., 1., 1., 20.);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(0., 3., 3., 0., 0., 0., 0., 1., 0.);	
 
 glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);
glEnableClientState (GL_TEXTURE_COORD_ARRAY_EXT);
model.draw();
glDisableClientState (GL_TEXTURE_COORD_ARRAY_EXT);
glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_VERTEX_ARRAY);
	//~ 

	// set the orthographic projection
	//~ glCallList(g_ortho);

	// 2D/text	*****************************************
	
	
	//~ glPopMatrix();

	// Commented out because we call it in our idle() function - maintains framerate independance
	// SDL_GL_SwapBuffers();
} 

int init()
{
	
	// OpenGL init
	// Stuff
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	
	// Face culling (for textures)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); 
	glEnable(GL_DEPTH_TEST);
	
	// Lighting
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	
	char pathToObj[] = "./models/MultiTextures.obj";
	// Object loader
	if(!model.load(pathToObj))
	{
		cout << "Could not load model" << endl;
		return false;
	}
	
	return true;
}
 
int main(int argc, char *argv[]) {

 dpy = XOpenDisplay(NULL);
 
 if(dpy == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
 }
        
 root = DefaultRootWindow(dpy);

 vi = glXChooseVisual(dpy, 0, att);

 if(vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
 } 
 else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
 }


 cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

 swa.colormap = cmap;
 swa.event_mask = ExposureMask | KeyPressMask;
 
 win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

 XMapWindow(dpy, win);
 XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
 
 glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
 glXMakeCurrent(dpy, win, glc);
 
 glEnable(GL_DEPTH_TEST); 
 
 while(1) {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose) {
                XGetWindowAttributes(dpy, win, &gwa);
                glViewport(0, 0, gwa.width, gwa.height);
                DrawAQuad(); 
                glXSwapBuffers(dpy, win);
        }
                
        else if(xev.type == KeyPress) {
                glXMakeCurrent(dpy, None, NULL);
                glXDestroyContext(dpy, glc);
                XDestroyWindow(dpy, win);
                XCloseDisplay(dpy);
                exit(0);
        }
    } /* this closes while(1) { */
} /* this is the } which closes int main(int */
