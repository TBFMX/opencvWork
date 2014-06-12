// -- Written in C -- //

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
// include generated arrays
//~ #include "./banana.h"
//~ #include "./sede.h"
#include "./rossini.h"
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

void DrawAQuad() {
 glClearColor(0.0, 0.0, 0.0, 0.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


// set input data to arrays
glVertexPointer(3, GL_FLOAT, 0, rossiniVerts);
glNormalPointer(GL_FLOAT, 0, rossiniNormals);
glTexCoordPointer(2, GL_FLOAT, 0, rossiniTexCoords);

glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_COLOR);
glEnableClientState (GL_TEXTURE_COORD_ARRAY);


// draw data
glDrawArrays(GL_TRIANGLES, 0, rossiniNumVerts);

// deactivate vertex arrays after drawing


glDisableClientState (GL_TEXTURE_COORD_ARRAY);
glDisableClientState(GL_COLOR);
glDisableClientState(GL_VERTEX_ARRAY);
//glDisableClientState (GL_TEXTURE_COORD_ARRAY_EXT); 
} 

//~ static GLfloat VERTICES[]={
		//~ -1.0f,  1.0f,  1.0f, //1
        //~ 1.0f,  1.0f,  1.0f, //2
        //~ 1.0f, -1.0f,  1.0f, //3
       //~ -1.0f, -1.0f,  1.0f, //4
//~ 
       //~ -1.0f,  1.0f, -1.0f, //5
        //~ 1.0f,  1.0f, -1.0f, //6
        //~ 1.0f, -1.0f, -1.0f, //7
       //~ -1.0f, -1.0f, -1.0f //8
//~ };

unsigned int cubeNumVerts = 36;

float cubeVerts[] =
{
1, -1, -1, 
1, 1, -0.999999, 
0.999999, 1, 1, 
1, -1, -1, 
0.999999, 1, 1, 
1, -1, 1, 
1, 1, -0.999999, 
1, -1, -1, 
-1, -1, -1, 
1, 1, -0.999999, 
-1, -1, -1, 
-1, 1, -1, 
1, -1, 1, 
0.999999, 1, 1, 
-1, 1, 1, 
1, -1, 1, 
-1, 1, 1, 
-1, -1, 1, 
1, -1, -1, 
1, -1, 1, 
-1, -1, 1, 
1, -1, -1, 
-1, -1, 1, 
-1, -1, -1, 
-1, -1, 1, 
-1, 1, 1, 
-1, 1, -1, 
-1, -1, 1, 
-1, 1, -1, 
-1, -1, -1, 
1, 1, -0.999999, 
-1, 1, -1, 
-1, 1, 1, 
1, 1, -0.999999, 
-1, 1, 1, 
0.999999, 1, 1, 
};

float cubeNormals[108] = 
{
1, 0, 0, 
1, 0, 0, 
1, 0, 0, 
1, 0, 0, 
1, 0, 0, 
1, 0, 0, 
0, 0, -1, 
0, 0, -1, 
0, 0, -1, 
0, 0, -1, 
0, 0, -1, 
0, 0, -1, 
-0, -0, 1, 
-0, -0, 1, 
-0, -0, 1, 
-0, -0, 1, 
-0, -0, 1, 
-0, -0, 1, 
0, -1, 0, 
0, -1, 0, 
0, -1, 0, 
0, -1, 0, 
0, -1, 0, 
0, -1, 0, 
-1, -0, -0, 
-1, -0, -0, 
-1, -0, -0, 
-1, -0, -0, 
-1, -0, -0, 
-1, -0, -0, 
0, 1, 0, 
0, 1, 0, 
0, 1, 0, 
0, 1, 0, 
0, 1, 0, 
0, 1, 0, 
};

static GLfloat COLORS[] = {
        1.0f, 0.0f, 0.0f, 1.0f, // red
        1.0f, 0.0f, 0.0f, 1.0f, // red
        1.0f, 0.0f, 0.0f, 1.0f, // red
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 0.0f, 1.0f, 1.0f, // blue
        0.0f, 0.0f, 1.0f, 1.0f, // blue
        0.0f, 0.0f, 1.0f, 1.0f, // blue
        1.0f, 0.0f, 0.0f, 1.0f, // red
        1.0f, 0.0f, 0.0f, 1.0f, // red
        1.0f, 0.0f, 0.0f, 1.0f, // red
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 1.0f, 0.0f, 1.0f, // green
        0.0f, 0.0f, 1.0f, 1.0f, // blue
        0.0f, 0.0f, 1.0f, 1.0f, // blue
        0.0f, 0.0f, 1.0f, 1.0f, // blue
};

void DrawCube(){
	float scale = 10.0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // transparent

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(-1., 1., -1., 1., 1., 100.);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(0., 3., 3., 0., 0., 0., 0., 1., 0.);	
    //glLoadMatrixf(pose, 0);
    glTranslatef(0.0f, 0.0f, -5.0f);
    //gl.glTranslatef(0f,0f,0f);
    glScalef(scale, scale, scale);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_COLOR);

	glVertexPointer(3, GL_FLOAT, 0, cubeVerts);
	//glNormalPointer(GL_FLOAT, 0, cubeNormals);
    glColorPointer(4, GL_FLOAT, 0, COLORS);

    glDrawArrays(GL_TRIANGLES,0,cubeNumVerts);
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
                //~ DrawAQuad();
                DrawCube();
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
