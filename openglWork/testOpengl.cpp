#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

void displayMe(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
// set input data to arrays
glVertexPointer(3, GL_FLOAT, 0, bananaVerts);
glNormalPointer(GL_FLOAT, 0, bananaNormals);
glTexCoordPointer(2, GL_FLOAT, 0, bananaTexCoords);

// draw data
glDrawArrays(GL_TRIANGLES, 0, bananaNumVerts);
    glFlush();
}

int main(int argc, char** argv)
{	
	InitializeAWindowPlease();
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0,1.0,1.0);
    glOrtho(0.0,1.0,0.0,1.0,-1.0, 1.0);
    glBegin(GL_POLYGON);
		glVertex3f(0.25, 0.25, 0.0);
		glVertex3f(0.75, 0.25, 0.0);
		glVertex3f(0.75, 0.75, 0.0);
		glVertex3f(0.25, 0.75, 0.0);
	glEnd();
	glFlush();
	//UpdateTheWindowAndCheckForEvents();
    return 0;
}
